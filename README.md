# Tweets Generation with Markov Chain
This program generates random sentences using a Markov chain algorithm. The program reads a given file and generates a dictionary of words and their corresponding probabilities. Then, it uses the probabilities to generate random sentences.

## Usage

To run the program, use the following command:
```
./text_generator <seed> <#sentences to generate> <path> [optional: <#words to read>]
```
1. seed: A random seed for the program.
2. #sentences to generate: The number of random sentences to generate.
3. path: The path to the file to read.
4. #words to read (optional): The number of words to read from the file. If not specified, the entire file will be read.

## Examples
Here are some examples of how to run the program:

```
./text_generator 454545 15 justdoit_tweets.txt
```
This will generate 5 random sentences using the file input.txt as the source of words, using the seed 123.
![image](https://user-images.githubusercontent.com/71530633/233385391-327cf17d-308a-473e-963f-98e4b0d034ca.png)

## Compilation

To compile the program, run the following command:

```
gcc text_generator.c -o text_generator
```
This will generate an executable file named text_generator.

## Acknowledgements
This program was created by Ido Pinto as part of the course "PROGRAMMING WORKSHOP IN C & C++" @ HUJI
