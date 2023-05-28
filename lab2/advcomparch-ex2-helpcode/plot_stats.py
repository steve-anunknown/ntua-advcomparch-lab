import matplotlib.pyplot as plt
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

        # Extract branch statistics
        branch_stats_match = re.search(r'Branch statistics:\s+Total-Branches: (\d+)\s+Conditional-Taken-Branches: (\d+)\s+Conditional-NotTaken-Branches: (\d+)\s+Unconditional-Branches: (\d+)\s+Calls: (\d+)\s+Returns: (\d+)', content)
        if branch_stats_match:
            stats['Total-Branches'] = int(branch_stats_match.group(1))
            stats['Conditional-Taken-Branches'] = int(branch_stats_match.group(2))
            stats['Conditional-NotTaken-Branches'] = int(branch_stats_match.group(3))
            stats['Unconditional-Branches'] = int(branch_stats_match.group(4))
            stats['Calls'] = int(branch_stats_match.group(5))
            stats['Returns'] = int(branch_stats_match.group(6))

    return stats

def plot_stats(filenames, output_path):
    stats = []
    x_labels = []
    for filename in filenames:
        stats.append(read_stats_from_file(filename))
        x_label = os.path.basename(filename).split('.')[0:2]
        x_labels.append('.'.join(x_label))

    sorted_labels, sorted_stats = zip(*sorted(zip(x_labels, stats), key=lambda x: x[1].get('Total-Instructions', 0)))
    y_values = ['Total-Instructions', 'Total-Branches', 'Conditional-Taken-Branches', 'Conditional-NotTaken-Branches', 'Unconditional-Branches', 'Calls', 'Returns']
    data = {label: [stat.get(label, 0) for stat in sorted_stats] for label in y_values}

    fig, ax = plt.subplots()
    for label in y_values:
        ax.plot(sorted_labels, data[label], label=label)

    ax.set_xticklabels(sorted_labels, rotation=45, ha='right')
    ax.set_xlabel('Benchmark')
    ax.set_ylabel('Count')
    ax.set_title('Branch Stats')
    ax.legend()

    plt.tight_layout()
    plt.savefig(output_path, dpi=300)
    print(f"Plot saved to: {output_path}")

# Specify the folder containing the files
folder_path = 'outputs/spec_execs_train_inputs/old_predictors_stats'

# Get a list of file paths in the folder matching the pattern
file_pattern = '*.out'
file_paths = glob.glob(os.path.join(folder_path, file_pattern))

# Check if any files are found
if not file_paths:
    print(f"No files found in folder '{folder_path}' matching the pattern '{file_pattern}'.")
else:
    output_path = "branch_stats.png"
    plot_stats(file_paths, output_path)

