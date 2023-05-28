import matplotlib.pyplot as plt
import re
import os
import glob
import matplotlib.cm as cm

def read_stats_from_file(filename):
    stats = {}
    with open(filename, 'r') as file:
        content = file.read()

        # Extract total instructions
        total_instructions_match = re.search(r'Total Instructions: (\d+)', content)
        if total_instructions_match:
            stats['Total-Instructions'] = int(total_instructions_match.group(1))

        # Extract branch predictors statistics
        branch_predictors_match = re.findall(r'(\w+): (\d+) (\d+)', content)
        if branch_predictors_match:
            stats['Branch-Predictors'] = {}
            for predictor, correct, incorrect in branch_predictors_match:
                stats['Branch-Predictors'][predictor] = {
                    'Correct': int(correct),
                    'Incorrect': int(incorrect)
                }

    return stats

def plot_branch_predictors_stats(filenames, output_path):
    stats = {}
    for filename in filenames:
        stats[filename] = read_stats_from_file(filename)

    # Sort filenames based on total instructions in ascending order
    sorted_filenames = sorted(stats, key=lambda x: stats[x]['Total-Instructions'])

    fig, ax = plt.subplots()
    colormap = cm.get_cmap('tab10')

    for i, filename in enumerate(sorted_filenames):
        file_stats = stats[filename]
        total_instructions = file_stats['Total-Instructions']

        branch_predictors_stats = file_stats.get('Branch-Predictors', {})
        for predictor, predictor_stats in branch_predictors_stats.items():
            correct = predictor_stats.get('Correct', 0)
            incorrect = predictor_stats.get('Incorrect', 0)
            mispredictions_per_kilo = incorrect / (total_instructions / 1000)

            color = colormap(i % colormap.N)
            ax.plot([predictor], [mispredictions_per_kilo], 'o-', label=filename, color=color)

    ax.set_xlabel('Predictors')
    ax.set_ylabel('Mispredictions per Kilo Instructions')
    ax.set_title('Mispredictions per Kilo Instructions for Branch Predictors')
    ax.grid(True)
    ax.legend()

    plt.tight_layout()
    plt.savefig(output_path, dpi=300)
    print(f"Plot saved to: {output_path}")

# Specify the folder containing the files
folder_path = '../outputs_original/statics-and-pentium'

# Get a list of file paths in the folder matching the pattern
file_pattern = '*.out'
file_paths = glob.glob(os.path.join(folder_path, file_pattern))

# Check if any files are found
if not file_paths:
    print(f"No files found in folder '{folder_path}' matching the pattern '{file_pattern}'.")
else:
    output_path = 'staticspentium.png'
    plot_branch_predictors_stats(file_paths, output_path)

