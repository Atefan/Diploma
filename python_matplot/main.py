import matplotlib.pyplot as plt
from collections import Counter

def plot_histogram(file_path):
    try:
        # Read the file
        with open(file_path, 'r') as file:
            values = [line.strip() for line in file.readlines()]
        
        # Count occurrences of each value
        value_counts = Counter(values)
        
        # Sort data by the keys (converting to integers if possible)
        sorted_items = sorted(value_counts.items(), key=lambda x: int(x[0]) if x[0].isdigit() else x[0])
        labels, counts = zip(*sorted_items)
        
        # Create the histogram
        plt.figure(figsize=(10, 6))
        plt.bar(labels, counts, color='skyblue', edgecolor='black')
        plt.xlabel('Values')
        plt.ylabel('Frequency')
        plt.title('Histogram of Value Frequencies')
        
        # Handle overlapping labels by displaying fewer x-ticks
        if len(labels) > 20:  # Adjust as needed based on your dataset
            plt.xticks(ticks=range(0, len(labels), len(labels) // 20), labels=[labels[i] for i in range(0, len(labels), len(labels) // 20)], rotation=45, ha='right')
        else:
            plt.xticks(rotation=45, ha='right')
        
        plt.tight_layout()
        plt.show()
    
    except FileNotFoundError:
        print(f"Error: The file '{file_path}' does not exist.")
    except Exception as e:
        print(f"An error occurred: {e}")

# Example usage
file_path = './../GUI/serial_data.txt'  # Replace with your file path
plot_histogram(file_path)
