import matplotlib.pyplot as plt

file_path = './noise.bin'

bit_counts = [0] * 8
total_bytes = 0

with open(file_path, 'rb') as f:
    byte = f.read(1)
    while byte:
        total_bytes += 1
        byte_value = ord(byte)
        for bit in range(8):
            if byte_value & (1 << bit):
                bit_counts[bit] += 1
        byte = f.read(1)

proportions = [count / total_bytes for count in bit_counts]

plt.figure(figsize=(8, 5))
plt.bar(range(8), proportions, color='skyblue', tick_label=[f'Bit {i}' for i in range(8)])
plt.xlabel('Bit Position (0 to 7)')
plt.ylabel('Proportion of 1s')
plt.title('Proportion of 1s for Each Bit Position')
plt.ylim(0, 1)
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.tight_layout()
plt.show()
