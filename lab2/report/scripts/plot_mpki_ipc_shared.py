import matplotlib.pyplot as plt
import matplotlib.cm as cm
import re
import os
import glob

def read_stats_from_file(filename):
    stats = {}
    with open(filename, 'r') as file:
        content = file.read()

        # Extract total instructions
        total_instructions_match = re.search(r'Total Instructions: (\d+)', content)
        if total_instructions_match:
            stats['Total-Instructions'] = int(total_instructions_match.group(1))

        ## Extract RAS entries mispredictions
        #ras_entries_match = re.findall(r'RAS \((\d+) entries\): (\d+) (\d+)', content)
        #if ras_entries_match:
        #    stats['RAS-Entries'] = {}
        #    for entries, correct, incorrect in ras_entries_match:
        #        stats['RAS-Entries'][entries] = {
        #            'Correct': int(correct),
        #            'Incorrect': int(incorrect)
        #        }

        ## Extract nbit predictors mispredictions
        #nbit_predictors_match = re.findall(r'Nbit-32K-([\d-]+[b]?): (\d+) (\d+)', content)
        #if nbit_predictors_match:
        #    stats['Nbit-Predictors'] = {}
        #    for entries, correct, incorrect in nbit_predictors_match:
        #        stats['Nbit-Predictors'][entries] = {
        #            'Correct': int(correct),
        #            'Incorrect': int(incorrect)
        #        }

        # Extract BTB predictors statistics
        btb_predictors_match = re.findall(r'([\w-]+): (\d+) (\d+) (\d+)', content)
        if btb_predictors_match:
            stats['BTB-Predictors'] = {}
            for predictor, correct, incorrect, target_correct in btb_predictors_match:
                stats['BTB-Predictors'][predictor.strip()] = {
                    'Correct': int(correct),
                    'Incorrect': int(incorrect),
                    'TargetCorrect': int(target_correct)
                }

        ## Extract branch predictors statistics
        #branch_predictors_match = re.findall(r'([^:\n]+)[:] (\d+) (\d+)', content)
        #if branch_predictors_match:
        #    stats['Predictor'] = {}
        #    for predictor, correct, incorrect in branch_predictors_match:
        #        stats['Predictor'][predictor.strip()] = {
        #            'Correct': int(correct),
        #            'Incorrect': int(incorrect)
        #        }

    return stats

def plot_mispredictions_per_kilo(stats, output_path):
    fig, ax = plt.subplots()
    legend_items = []
    # Get the maximum total instructions to normalize the colormap
    max_total_instructions = max([file_stats.get('Total-Instructions', 0) for file_stats in stats.values()])


    for filename, file_stats in stats.items():
        total_instructions = file_stats.get('Total-Instructions', 0)
        nbit_predictors = file_stats.get('BTB-Predictors', {})
        nbit_entries = []
        mispredictions_per_kilo = []

        for entries, predictor_stats in nbit_predictors.items():
            correct = predictor_stats.get('Correct', 0)
            incorrect = predictor_stats.get('Incorrect', 0)
            mispredictions_per_kilo.append(incorrect / (total_instructions / 1000))
            nbit_entries.append(entries)

        # Extract the desired portion of the filename
        short_filename = '.'.join(os.path.basename(filename).split('.')[:2])
        legend_items.append((short_filename, total_instructions, nbit_entries, mispredictions_per_kilo))

    # Sort legend items based on total instructions in ascending order
    legend_items.sort(key=lambda x: x[1])

    # Create a colormap with a sequential color scheme
    colormap = cm.get_cmap('tab10')

    #for short_filename, total_instructions, nbit_entries, mispredictions_per_kilo in legend_items:
    #    ax.plot(nbit_entries, mispredictions_per_kilo, 'o-', label=short_filename)

    for i, (short_filename, total_instructions, nbit_entries, mispredictions_per_kilo) in enumerate(legend_items):
        color = colormap(total_instructions / max_total_instructions)
        ax.plot(nbit_entries, mispredictions_per_kilo, 'o-', label=short_filename, color=color)


    ax.set_xlabel('BTB-Predictor')
    ax.tick_params(axis='x', labelrotation=20)
    ax.set_ylabel('Mispredictions per Kilo Instructions')
    ax.set_title('Mispredictions per Kilo Instructions for Branch Predictors')
    ax.grid(True)
    fig.set_size_inches(14, 8)
    fig.subplots_adjust(bottom=0.2)
    # Adjust the legend positioning
    ax.legend()
    #bbox_to_anchor=(1.02, 1), loc='upper left'

    #plt.tight_layout()
    plt.savefig(output_path, dpi=300)
    print(f"Plot saved to: {output_path}")


# Specify the folder containing the files
folder_path = '../outputs_original/btb'

# Get a list of file paths in the folder matching the pattern
file_pattern = '*.out'
file_paths = glob.glob(os.path.join(folder_path, file_pattern))

# Check if any files are found
if not file_paths:
    print(f"No files found in folder '{folder_path}' matching the pattern '{file_pattern}'.")
else:
    stats = {}
    for file_path in file_paths:
        filename = os.path.basename(file_path)
        file_stats = read_stats_from_file(file_path)
        stats[filename] = file_stats

    output_path = 'btb.png'
    plot_mispredictions_per_kilo(stats, output_path)

