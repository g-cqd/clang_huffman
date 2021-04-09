# Huffman encoding, C Language Implementation

## Presentation of the Huffman encoding system

1. Write a command to compress a file according to the HUFFMAN code.
2. Write the corresponding decompression command.

## Details

- The input alphabet of the code can be made up of characters of a fixed size of one byte.
- The enwik[5,6,7...] files are good candidates to test the program.
- At the head of the compressed file, it is necessary to store the chosen HUFFMAN encoding.

## Progressive improvement tracks

1. For a given file, find the optimum for the fixed size of the elements of the the input alphabet.
2. Introduce the notion of context and transform your coding into an adaptive adaptive huffman coding for a fixed context size of one byte.
3. For a given file, find the optimal for the fixed context size.

## Commit History on [GerritHub](https://gerrithub.io/c/ADSCCD-2020-2021/compression_huffman/)

<details>

<summary><i>Open to see history</i></summary>

<div>

### Seventh Step: Unzip -- First GitHub Commit

- First decompression -- padding not yet understand -- bug with large file

### Sixth Step: Compression is Working

- Compression gives an .hf file with understandable headers and padding
- No bug known currently
- Compression ratio is rather 35% than previoulsy announced 50%
- Quite fast, writing bottleneck remains
- Code refactoring
- Better error handling

### Fifth Step: First Compression

- Compression State: .huf generated with header informations
- Header: `(<length>[;<char>,<weight>,<encoded bit-sized length>,<value>]*)<encoded file>`
- Estimated compression ratio: 50%
- Sorry for being late

### Fourth Step: Taking a step back

- Improved Performance: Thanks to P. LUCAS
- Alleged Code: Unnecessary code previously inserted to get perf boost
  but clearly useless now
- malloc testing: Thanks to Dridri
- Read Bytes Fixed -- maybe due to variable type confusion
- Better string functions: Thanks to Dridri
- Better Handling of Monkeys Army (CLI Parameters Checker): Thanks to
  Dridri
- enwik9 added as Test 4 --> 1GB (1 000 000 000 B)
- enwik3 is Test 3 --> 100MB (100 000 000 B)

### Second Step: Reliability

- Counting: OK
- Sorting: HeapSort: OK
- Sorting: MergeSort: OK
- Tree Building: OK
- Data Visualization: OK
- Memory Release: OK -> Adrien's Suggestion (Fixed)
- Encoding: NEXT

### First Step: Initialization

- Count occurrences by 8-bit chars

</div>
</details>
