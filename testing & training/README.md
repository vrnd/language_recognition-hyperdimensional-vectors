This Matlab code implements a language recognition algorithm using hyperdimensional computing. 
Basically there are 2 main functions:

1. buildLanguageHV (N, D): that is a training function. 
D is the dimension of hypervectors (in the order of 10K) and N is the size of N-grams (from unigrams to e.g., pentagrams).
This function returns [iM, langAM]. iM is an item memory where hypervectors are stored. 
langAM is a memory where language hypervectors are stored and can be used as an associative memory.


2. test (iM, langAM, N, D): that is a test function.
This test function tests unseen sentences and tries to recognizes their languages by querying into langAM.
 
Here is a simple example of using algorithm:

>> langRecognition
>> D = 10000;
>> N = 4;
>> [iM, langAM] = buildLanguageHV (N, D);
Loaded traning language file ../training_texts/afr.txt
Loaded traning language file ../training_texts/bul.txt
... 
%Please be patient it will take a while to lead all languages

>> accuracy = test (iM, langAM, N, D)
Loaded testing text file ../testing_texts/pl_715_p.txt
Loaded testing text file ../testing_texts/pl_716_p.txt
....
accuracy =  0.9783







Please let us know if you face any problem or discover any bugs!
For more info, you can read our paper, "A Robust and Energy-Efficient Classifier Using Brain-Inspired Hyperdimensional Computing" at ISLPED 2016.

Thanks!
email: abbas@eecs.berkeley.edu
