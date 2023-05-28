import matplotlib.pyplot as plt
import matplotlib.cm as cm
import re
import os
import glob

def read_ras_stats_from_file(filename):
    stats = {}
    with open(filename, 'r') as file:
        content = file.read()

        # Extract total instructions
        total_instructions_match = re.search(r'Total Instructions: (\d+)', content)
        if total_instructions_match:
            stats['Total-Instructions'] = int(total_instructions_match.group(1))

        # Extract RAS entries statistics
        ras_entries_match = re.findall(r'RAS \((\d+) entries\): (\d+) (\d+)', content)
        if ras_entries_match:
            stats['RAS-Entries'] = {}
            for entries, correct, incorrect in ras_entries_match:
                stats['RAS-Entries'][entries] = {
                    'Correct': int(correct),
                    'Incorrect': int(incorrect)
                }

    return stats

def plot_ras_stats(filenames, output_path):
    stats = []
    x_labels = []
    for filename in filenames:
        stats.append(read_ras_stats_from_file(filename))
        # Extract the desired portion of the filename
        x_label = '.'.join(os.path.basename(filename).split('.')[:2])
        x_labels.append(x_label)

    ras_entries = []
    mispredictions_per_kilo = []
    total_instructions_list = []

    for stat in stats:
        total_instructions = stat.get('Total-Instructions', 0)
        total_instructions_list.append(total_instructions)
        ras_entries_stats = stat.get('RAS-Entries', {})
        for entries, ras_stat in ras_entries_stats.items():
            correct = ras_stat.get('Correct', 0)
            incorrect = ras_stat.get('Incorrect', 0)
            mispredictions_per_kilo.append(incorrect / (total_instructions / 1000))
            ras_entries.append(entries)

    # Sort the data based on the number of RAS entries
    sorted_data = sorted(zip(ras_entries, mispredictions_per_kilo), key=lambda x: int(x[0]))
    ras_entries, mispredictions_per_kilo = zip(*sorted_data)

    fig, ax = plt.subplots()
    colormap = cm.get_cmap('tab10')

    legend_items = []
    for ras_entry, mispredictions, total_instructions in zip(ras_entries, mispredictions_per_kilo, total_instructions_list):
        color = colormap(total_instructions / max(total_instructions_list))
        line, = ax.plot(ras_entry, mispredictions, 'o-', color=color)
        legend_items.append((line, total_instructions))

    # Sort the legend items based on total instructions in ascending order
    legend_items.sort(key=lambda x: x[1])
    handles, labels = zip(*legend_items)

    ax.set_xlabel('RAS Entries')
    ax.set_ylabel('Mispredictions per Kilo Instructions')
    ax.set_title('Mispredictions per Kilo Instructions for RAS')
    ax.grid(True)

    # Set the shortened filenames as the legend labels
    shortened_labels = ['.'.join(label.split('.')[:2]) for label in x_labels]
    ax.legend(handles, shortened_labels, title='Total Instructions', loc='best')

    plt.tight_layout()
    plt.savefig(output_path, dpi=300)
    print(f"Plot saved to: {output_path}")

# Specify the folder containing the files
folder_path = '../outputs_original/ras'

# Get a list of file paths in the folder matching the pattern
file_pattern = '*.out'
file_paths = glob.glob(os.path.join(folder_path, file_pattern))

# Check if any files are found
if not file_paths:
    print(f"No files found in folder '{folder_path}' matching the pattern '{file_pattern}'.")
else:
    output_path = 'ras.png'
    plot_ras_stats(file_paths, output_path)

