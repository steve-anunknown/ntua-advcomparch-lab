
class BTBPredictor : public BranchPredictor
{
public:
	BTBPredictor(int btb_lines, int btb_assoc)
	     : table_lines(btb_lines), table_assoc(btb_assoc)
	{
		btb = new ADDRINT*[table_lines/table_assoc];
		for (int i = 0; i < table_lines/table_assoc; i++) {
			btb[i] = new ADDRINT[table_assoc]();
		}
	}

	~BTBPredictor() {
		for (int i = 0; i < table_lines; i++) {
			delete[] btb[i];
		}
		delete[] btb;
	}

    virtual bool predict(ADDRINT ip, ADDRINT target) {
		int index = ip % (table_lines/table_assoc);
		for (int i = 0; i < table_assoc; i++) {
			if (btb[index][i] == target) {
				return true;
			}
		}
		return false;
	}

    virtual void update(bool predicted, bool actual, ADDRINT ip, ADDRINT target) {
		if (!predicted || actual) {
			int index = ip % table_lines;
			for (int i = 0; i < table_assoc; i++) {
				if (btb[index][i] == 0) {
					btb[index][i] = target;
					break;
				}
			}
		}
	}

    virtual string getName() { 
        std::ostringstream stream;
		stream << "BTB-" << table_lines << "-" << table_assoc;
		return stream.str();
	}

    UINT64 getNumCorrectTargetPredictions() { 
		UINT64 numCorrectPredictions = 0;
		for (int i = 0; i < table_lines; i++) {
			for (int j = 0; j < table_assoc; j++) {
				if (btb[i][j] != 0) {
					numCorrectPredictions++;
				}
			}
		}
		return numCorrectPredictions;
	}

private:
	int table_lines, table_assoc;
	ADDRINT** btb;
};
