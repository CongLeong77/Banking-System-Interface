Project Title
Coursework 1 - C Programming (Banking System)

Created by
Leong Ngai Cong (36591149)

Project Description
This is a rudimentary banking system coded in C which can create an account,
deposit money, withdraw money, send remittance from one account to another,
and delete accounts from a directory.

How To Use The Project
Launch CW1_BankingSystem.exe which should be in the same folder as this readme.txt

Tests
The banking system code should be able to handle:
	1. Non-numeric Input 
		When prompted to enter a monetary value, the program accepts only floats
		When given any non-float input, it throws up an error and prompts for a proper input
	
	2. Insufficient Funds
		When the user asks to withdraw more money than is available in the account, the program throws an error stops the function
		When the user asks to withdraw a negative amount, the program throws an error and stops the function

	3. Invalid Option in Menu
		When the user enters anything other than the menu options accepted, the program throws an error and keeps prompting the user for a valid input
		Program should only accept numbers 1, 2, 3, 4, 5, -1 (for exit).

