import struct
from collections import Counter

def sort_and_compare_int32_files(input_path, sorted_path):
    try:
        # Read and sort the input file
        with open(input_path, 'rb') as input_file:
            input_data = input_file.read()
            if len(input_data) % 4 != 0:
                print(f"Warning: Input file size {len(input_data)} is not multiple of 4")
            input_ints = struct.unpack(f'{len(input_data)//4}i', input_data)
            sorted_ints = sorted(input_ints, reverse=True)

        # Read the target sorted file
        with open(sorted_path, 'rb') as sorted_file:
            sorted_data = sorted_file.read()
            if len(sorted_data) % 4 != 0:
                print(f"Warning: Sorted file size {len(sorted_data)} is not multiple of 4")
            target_ints = struct.unpack(f'{len(sorted_data)//4}i', sorted_data)

        # Length check
        len_match = len(sorted_ints) == len(target_ints)

        # Compare contents using Counter for frequency analysis
        counter1 = Counter(sorted_ints)
        counter2 = Counter(target_ints)

        if len_match and counter1 == counter2:
            print("SUCCESS: Files contain exactly the same integers")
            return True

        # Handle length mismatches
        if not len_match:
            print(f"Length mismatch: {len(sorted_ints)} vs {len(target_ints)} integers")

            # Find missing/extras
            missing_in_sorted = list((counter2 - counter1).elements())
            missing_in_target = list((counter1 - counter2).elements())

            if missing_in_sorted:
                print(f"\nIntegers in sorted.bin but NOT in input (total: {len(missing_in_sorted)}):")
                print_sample(missing_in_sorted)

            if missing_in_target:
                print(f"\nIntegers in input but NOT in sorted.bin (total: {len(missing_in_target)}):")
                print_sample(missing_in_target)

        # Show position-based differences (only if lengths match)
        if len_match and counter1 != counter2:
            print("\nFiles have same length but different content:")
            differences = []
            for i, (a, b) in enumerate(zip(sorted_ints, target_ints)):
                if a != b:
                    differences.append((i, a, b))
                    if len(differences) > 10:
                        break
            for idx, actual, expected in differences:
                print(f"Position {idx}: {actual} vs {expected}")

        return False

    except Exception as e:
        print(f"Error: {str(e)}")
        return False

def print_sample(items, max_samples=10):
    """Helper to print sample items"""
    if len(items) <= max_samples:
        print(items)
    else:
        print(f"First {max_samples} samples: {items[:max_samples]}")
        print(f"(Plus {len(items)-max_samples} more...)")

input_file = '../cmake-build-debug/tape.bin'
sorted_file = '../cmake-build-debug/sorted.bin'

sort_and_compare_int32_files(input_file, sorted_file)