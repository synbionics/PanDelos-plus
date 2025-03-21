import argparse
import os
import subprocess


def arg_parser():
    parser = argparse.ArgumentParser(description="Program to process files with output options.")
    
    parser.add_argument("-i", "--input", required=True, help="input file to process")
    parser.add_argument("-o", "--output", required=True, help="output folder for results")
    parser.add_argument("-c", "--core", required=True, help="core threshold, the minimun number of genomes that a gene must be present in to be considered core")
    parser.add_argument("-f", "--format", choices=["none", "list", "multifasta", "all"], 
                        default="none", help="format of files to generate in addition to graphs")
    args = parser.parse_args()
    return args

def main():
    script_root = "scripts/query_dataset/"
    
    args = arg_parser()
    
    if not os.path.isfile(args.input):
        print(f"Error: Input file '{args.input}' does not exist.")
        return
    
    if not args.input.endswith(".json"):
        print("Input file must be a json file")
        exit(1)
    
    if not os.path.exists(args.output):
        os.makedirs(args.output)
        print(f"Output folder '{args.output}' has been created.")
    
    print(f"Processing file '{args.input}'...")
    print(f"Results will be saved to '{args.output}'")

    
    # execute python3 ./query_dataset/core.py arg.input arg.output arg.core arg.format
    try:
        print("Running core analysis...")
        
        parts = [
            "python3", 
            f"{script_root}core.py", 
            args.input,
            args.output,
            args.core
        ]
        if args.format and args.format != "none":
            parts.append(args.format)
        
        subprocess.run(parts, check=True)
        print("Core analysis completed successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error executing core.py: {e}")
        exit(1)
    
    # execute python3 ./query_dataset/diffusivity.py arg.input arg.output arg.core arg.format
    try:
        print("Running diffusivity analysis...")
        
        parts = [
            "python3", 
            f"{script_root}diffusivity.py", 
            args.input,
            args.output
        ]
        if args.format and args.format != "none":
            parts.append(args.format)
        
        subprocess.run(parts, check=True)
        print("Diffusivity analysis completed successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error executing diffusivity.py: {e}")
        exit(1)
    
    # execute python3 ./query_dataset/pres_abs_matrix.py arg.input arg.output
    try:
        print("Generating presence/absence matrix...")
        subprocess.run([
            "python3", 
            f"{script_root}pres_abs_matrix.py", 
            args.input,
            args.output
        ], check=True)
        print("Presence/absence matrix generated successfully.")
    except subprocess.CalledProcessError as e:
        print(f"Error executing pres_abs_matrix.py: {e}")
        exit(1)
    
    print("All processing completed successfully.")
    

if __name__ == "__main__":
    main()