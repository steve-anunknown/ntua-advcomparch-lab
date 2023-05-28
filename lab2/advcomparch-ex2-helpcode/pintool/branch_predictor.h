#ifndef BRANCH_PREDICTOR_H
#define BRANCH_PREDICTOR_H

#include <sstream> // std::ostringstream
#include <cmath>   // pow()
#include <cstring> // memset()
#include <vector> // std::vector

/**
 * A generic BranchPredictor base class.
 * All predictors can be subclasses with overloaded predict() and update()
 * methods.
 **/

class BranchPredictor
{
public:
    BranchPredictor() : correct_predictions(0), incorrect_predictions(0) {};
    ~BranchPredictor() {};

    virtual bool predict(ADDRINT ip, ADDRINT target) = 0;
    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) = 0;
    virtual string getName() = 0;

    UINT64 getNumCorrectPredictions() { return correct_predictions; }
    UINT64 getNumIncorrectPredictions() { return incorrect_predictions; }

    void resetCounters() { correct_predictions = incorrect_predictions = 0; };

protected:
    void updateCounters(bool predicted, bool actual) {
        if (predicted == actual)
            correct_predictions++;
        else
            incorrect_predictions++;
    };

private:
    UINT64 correct_predictions;
    UINT64 incorrect_predictions;
};

class NbitPredictor : public BranchPredictor
{
public:
    NbitPredictor(unsigned index_bits_, unsigned cntr_bits_)
        : BranchPredictor(), index_bits(index_bits_), cntr_bits(cntr_bits_) {
        table_entries = 1 << index_bits;
        TABLE = new unsigned long long[table_entries];
        memset(TABLE, 0, table_entries * sizeof(*TABLE));
        
        COUNTER_MAX = (1 << cntr_bits) - 1;
    };
    ~NbitPredictor() { delete TABLE; };

    virtual bool predict(ADDRINT ip, ADDRINT target) {
        unsigned int ip_table_index = ip % table_entries;
        unsigned long long ip_table_value = TABLE[ip_table_index];
        unsigned long long prediction = ip_table_value >> (cntr_bits - 1);
        return (prediction != 0);
    };

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
        unsigned int ip_table_index = ip % table_entries;
        if (actual) {
            if (TABLE[ip_table_index] < COUNTER_MAX)
                TABLE[ip_table_index]++;
        } else {
            if (TABLE[ip_table_index] > 0)
                TABLE[ip_table_index]--;
        }
        
        updateCounters(predicted, actual);
    };

    virtual string getName() {
        std::ostringstream stream;
        stream << "Nbit-" << pow(2.0,double(index_bits)) / 1024.0 << "K-" << cntr_bits;
        return stream.str();
    }

private:
    unsigned int index_bits, cntr_bits;
    unsigned int COUNTER_MAX;
    
    /* Make this unsigned long long so as to support big numbers of cntr_bits. */
    unsigned long long *TABLE;
    unsigned int table_entries;
};

class TwobbitPredictor: public BranchPredictor
{
public:
    TwobbitPredictor(unsigned index_bits_)
        : BranchPredictor(), index_bits(index_bits_) {
        table_entries = 1 << index_bits;
        TABLE = new unsigned long long[table_entries];
        memset(TABLE, 0, table_entries * sizeof(*TABLE));
        
        COUNTER_MAX = (1 << 2) - 1;
    };
    
    ~TwobbitPredictor() { delete TABLE; };

    virtual bool predict(ADDRINT ip, ADDRINT target) {
        unsigned int ip_table_index = ip % table_entries;
        unsigned long long ip_table_value = TABLE[ip_table_index];
        unsigned long long prediction = ip_table_value >> (2 - 1);
        return (prediction != 0);
    };

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
        unsigned int ip_table_index = ip % table_entries;
        if (actual) {
            if (TABLE[ip_table_index] == 0)
                TABLE[ip_table_index]++;
            else TABLE[ip_table_index] = COUNTER_MAX;
        } else {
            if (TABLE[ip_table_index] == COUNTER_MAX)
                TABLE[ip_table_index]--;
            else TABLE[ip_table_index] = 0;
        }
        
        updateCounters(predicted, actual);
    };

    virtual string getName() {
        std::ostringstream stream;
        stream << "Nbit-" << pow(2.0,double(index_bits)) / 1024.0 << "K-" << 2 << 'b';
        return stream.str();
    }

private:
    unsigned int index_bits;
    unsigned int COUNTER_MAX;
    
    /* Make this unsigned long long so as to support big numbers of cntr_bits. */
    unsigned long long *TABLE;
    unsigned int table_entries;
};

class BTBPredictor : public BranchPredictor
{
public:
	BTBPredictor(int btb_lines, int btb_assoc)
    : table_lines(btb_lines), table_assoc(btb_assoc),
    correct_target_predictions(0), incorrect_target_predictions(0), timestamp(0)
	{
        total = table_lines*table_assoc;
		btb.resize(total);
        for (int i = 0; i < table_lines; i++) {
			btb[i].resize(table_assoc, std::make_tuple(0, 0, 0));
		}
	}

	~BTBPredictor() { btb.clear(); }

    virtual bool predict(ADDRINT ip, ADDRINT target)
    {
        int index = ip % table_lines;
		for (auto &entry : btb[index])
        {
			if (std::get<0>(entry) == ip)
            {
                std::get<2>(entry) = timestamp++;
                return std::get<1>(entry) == target;
            }
		}
		return false;
	}

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
		int index = ip % table_lines;
        if (actual && predicted)
        {
            for (auto &entry : btb[index])
            {
                if (std::get<0>(entry) == ip)
                {
                    correct_target_predictions += (std::get<1>(entry) == target);
                    incorrect_target_predictions += (std::get<1>(entry) != target);
                    std::get<1>(entry) = target;
                }
            }
        }
		// if not found, replace the least recently used entry
        else if (actual && !predicted)
        {
            int min = 0;
            for (int i = 0; i < table_assoc; i++)
            {
                if (std::get<2>(btb[index][i]) < std::get<2>(btb[index][min]))
                    min = i;
            }
            std::get<0>(btb[index][min]) = ip;
            std::get<1>(btb[index][min]) = target;
            std::get<2>(btb[index][min]) = timestamp++;
        }
        else if (!actual && predicted)
        {
            for (auto &entry: btb[index])
            {
                if (std::get<0>(entry) == ip)
                {
                    std::get<0>(entry) = 0;
                    std::get<1>(entry) = 0;
                    std::get<2>(entry) = 0;
                }
            }
        }
        updateCounters(predicted, actual);
	}

    virtual string getName() { 
        std::ostringstream stream;
		stream << "BTB-" << table_lines << "-" << table_assoc;
		return stream.str();
	}

    UINT64 getNumCorrectTargetPredictions() { return correct_target_predictions; }
	UINT64 getNumIncorrectTargetPredictions() { return incorrect_target_predictions; }

private:
	int table_lines, table_assoc, total;
    UINT64 correct_target_predictions;
    UINT64 incorrect_target_predictions;
    UINT64 timestamp;
    // The Branch Target Buffer
    // Each entry is a tuple of (ip, target, counter)
    // the counter is used in order to implement LRU replacement
	std::vector<std::vector<std::tuple<ADDRINT, ADDRINT, UINT64>>> btb; 
};


class StaticAlwaysTakenPredictor: public BranchPredictor
{
public:
    StaticAlwaysTakenPredictor(): BranchPredictor() {};
    
    ~StaticAlwaysTakenPredictor() {};

    virtual bool predict(ADDRINT ip, ADDRINT target)
    {
        return true;
    };

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target)
    {
        updateCounters(predicted, actual);
    };

    virtual string getName()
    {
        std::ostringstream stream;
        stream << "Static AlwaysTaken";
        return stream.str();
    }
};

class StaticBTFNTPredictor: public BranchPredictor
{
public:
    StaticBTFNTPredictor(): BranchPredictor() {};
    
    ~StaticBTFNTPredictor() {};

    virtual bool predict(ADDRINT ip, ADDRINT target)
    {
        return (target < ip);
    };

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target)
    {
        updateCounters(predicted, actual);
    };

    virtual string getName()
    {
        std::ostringstream stream;
        stream << "Static BTFNT";
        return stream.str();
    }
};

class LocalHistoryTwoLevelPredictor: public BranchPredictor
{
public:
    LocalHistoryTwoLevelPredictor(unsigned int pht_bits_, unsigned int pht_entries_,
                                   unsigned int bht_bits_, unsigned int bht_entries_)
    : BranchPredictor(), pht_bits(pht_bits_), bht_bits(bht_bits_), pht_entries(pht_entries_), bht_entries(bht_entries_)
    {
        COUNTER_MAX = (1 << pht_bits) - 1;
        CAP = 1 << bht_bits;

        BHT = new unsigned long long [bht_entries];
        memset(BHT, 0, bht_entries * sizeof(*BHT));

        PHT = new unsigned long long [pht_entries];
        memset(PHT, 0, pht_entries * sizeof(*PHT));
    };
    
    ~LocalHistoryTwoLevelPredictor()
    {

        delete BHT;
        delete PHT;
    };

    virtual bool predict(ADDRINT ip, ADDRINT target)
    {
        unsigned int bht_table_index = ip % bht_entries;
        unsigned long long bht_table_value = BHT[bht_table_index];
        unsigned long long pht_table_index = (((ip % pht_entries) << (bht_bits)) + bht_table_value) % pht_entries;
        unsigned long long prediction = (PHT[pht_table_index] >> (pht_bits - 1));
        return (prediction != 0);
    };

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target)
    {
        unsigned int bht_table_index = ip % bht_entries;
        unsigned long long bht_table_value = BHT[bht_table_index];
        unsigned long long pht_table_index = (((ip % pht_entries) << (bht_bits)) + bht_table_value) % pht_entries;
        // update pht for specific pattern
        if (actual) {
            if (PHT[pht_table_index] < COUNTER_MAX)
                PHT[pht_table_index]++;
        } else {
            if (PHT[pht_table_index] > 0)
                PHT[pht_table_index]--;
        }
        // update bht for specific branch
        BHT[bht_table_index] = ((actual << bht_bits) + BHT[bht_table_index]) >> 1;
        updateCounters(predicted, actual);
    };

    virtual string getName()
    {
        std::ostringstream stream;
        stream << "LocalHistoryTwoLevel: PHT Entries = " << pht_entries
               << ", PHT n-bit counter length = " << pht_bits
               << ", BHT Entries = " << bht_entries
               << ", BHT Entry length = " << bht_bits;
        return stream.str();
    }
private:
    unsigned int pht_bits, bht_bits;
    unsigned int pht_entries, bht_entries;
    unsigned int COUNTER_MAX, CAP;
    unsigned long long *BHT;
    unsigned long long *PHT;
};

class GlobalHistoryTwoLevelPredictor: public BranchPredictor
{
public:
    GlobalHistoryTwoLevelPredictor(unsigned int pht_entries_, unsigned int pht_bits_, unsigned int bhr_length_)
    : BranchPredictor(), pht_bits(pht_bits_), pht_entries(pht_entries_), bhr_length(bhr_length_)
    {
        COUNTER_MAX = (1 << pht_bits) - 1;
        BHR = 0;
        PHT = new unsigned long long [pht_entries];
        memset(PHT, 0, pht_entries * sizeof(*PHT));
    };
    
    ~GlobalHistoryTwoLevelPredictor()
    {
        delete PHT;
    };

    virtual bool predict(ADDRINT ip, ADDRINT target)
    {
        unsigned int pht_table_index = ((BHR * (pht_entries >> bhr_length)) + (ip % (pht_entries >> bhr_length))) % pht_entries;
        unsigned long long pht_table_value = PHT[pht_table_index];
        unsigned long long prediction = (pht_table_value >> (pht_bits - 1));
        return (prediction != 0);
    };

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target)
    {
        unsigned int pht_table_index = ((BHR * (pht_entries >> bhr_length)) + (ip % (pht_entries >> bhr_length))) % pht_entries;
        // update pht for specific pattern
        if (actual) {
            if (PHT[pht_table_index] < COUNTER_MAX)
                PHT[pht_table_index]++;
        } else {
            if (PHT[pht_table_index] > 0)
                PHT[pht_table_index]--;
        }
        // update bhr
        // shift the prediction to the left by history length bits
        // add it to the register (it's like you are queuing the prediction)
        // and then shift the whole thing to the right by one bit.
        BHR = ((actual << bhr_length) + BHR) >> 1;
        updateCounters(predicted, actual);
    };

    virtual string getName()
    {
        std::ostringstream stream;
        stream << "GlobalHistoryTwoLevel: PHT Entries = " << pht_entries
               << ", PHT n-bit counter length = " << pht_bits
               << ", BHR Length = " << bhr_length;
        return stream.str();
    };

private:
    unsigned int pht_bits, pht_entries, bhr_length;
    unsigned int COUNTER_MAX;
    unsigned long long BHR;
    unsigned long long *PHT;
};

class TournamentHybridPredictor: public BranchPredictor
{
public:
    TournamentHybridPredictor(unsigned int meta_entries_, unsigned int cntr_bits_, BranchPredictor* p0, BranchPredictor* p1)
    : meta_entries(meta_entries_), cntr_bits(cntr_bits_), predictor0(p0), predictor1(p1)
    {
        TABLE = new unsigned long long[meta_entries];
        memset(TABLE, 0, meta_entries * sizeof(*TABLE));
        
        COUNTER_MAX = (1 << cntr_bits) - 1;

    };
    ~TournamentHybridPredictor() {delete TABLE;};
    virtual bool predict(ADDRINT ip, ADDRINT target) {
        unsigned int ip_table_index = ip % meta_entries;
        unsigned long long ip_table_value = TABLE[ip_table_index];
        bool predictor = ip_table_value >> (cntr_bits - 1);
        prediction0 = predictor0->predict(ip, target);
        prediction1 = predictor1->predict(ip, target);
        return (!predictor)*(prediction0) + (predictor)*(prediction1);
    };

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
        updateCounters(predicted, actual);
        predictor0->update(prediction0, actual, ip, target);
        predictor1->update(prediction1, actual, ip, target);

        if (prediction0 == prediction1) return ;

        unsigned int ip_table_index = ip % meta_entries;
        if (actual == prediction0) {
            if (TABLE[ip_table_index] < COUNTER_MAX)
                TABLE[ip_table_index]++;
        } else {
            if (TABLE[ip_table_index] > 0)
                TABLE[ip_table_index]--;
        }
    };

    virtual string getName() {
        std::ostringstream stream;
        stream << "TournamentHybridPredictor-" << meta_entries / 1024.0 << "K-Entries-("
               << predictor0->getName() << ", " << predictor1->getName() << ")";
        return stream.str();
    }

private:
    /* Make this unsigned long long so as to support big numbers of cntr_bits. */
    unsigned long long *TABLE;
    unsigned int meta_entries;
    unsigned int COUNTER_MAX, cntr_bits;
    BranchPredictor *predictor0;
    BranchPredictor *predictor1;
    bool prediction0, prediction1;
};

#endif
