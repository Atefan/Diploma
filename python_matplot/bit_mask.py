import matplotlib.pyplot as plt
import numpy as np

def process_binary_file(file_path):
    try:
        # Read the binary file
        with open(file_path, "rb") as f:
            data = f.read()

        if not data:
            print("The file is empty.")
            return

        # Count occurrences of each byte value (0-255)
        byte_values, counts = np.unique(np.frombuffer(data, dtype=np.uint8), return_counts=True)

        # Create a full histogram with 256 bins (include empty values for missing bytes)
        full_counts = np.zeros(256, dtype=int)
        full_counts[byte_values] = counts

        # Plot the histogram
        plt.bar(range(256), full_counts, color='blue', alpha=0.7, width=1.0)
        plt.xticks(range(0, 256, 16), [f"{i}" for i in range(0, 256, 16)])  # Label every 16th bin for readability
        plt.xlabel("Byte Value (0-255)")
        plt.ylabel("Frequency")
        plt.title(f"Histogram of Byte Values (Total bytes: {len(data)})")
        plt.grid(axis="y", linestyle="--", alpha=0.7)
        plt.show()

        print(f"Number of bytes analyzed: {len(data)}")

    except FileNotFoundError:
        print("File not found. Please check the file path.")
    except Exception as e:
        print(f"An error occurred: {e}")

process_binary_file('./../GUI/bit_data')