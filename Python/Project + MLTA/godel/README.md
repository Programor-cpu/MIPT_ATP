This is attempts of gedol primitive recursive fuctions interpretator

In rules, counting starts from one

Syntax of rules:

i, j - numbers.
Zero functions is Z.
Number is number.
Projector function is Pi (P1 by default) which means "return argument number i".
Succesor func is Si^j (S1^1 by deafult) which means "return j + argument number i".
To use composition rule, just list all inner functions separated by commas.
Nesting supported.
To use Recursion rule list functions throw semicolon

All lines are either:
1) A test of function - func name followed by args.
	Boom(1, 3, 0)
2) A free line
	#once we will treat comments, hopefully
3) An announcement of a func - func name, '=', function value.
	Boom=S3(P3, P2, P1)
4) import - name of file from Code folder. Proceed every line. Let observe every line.
5) Easter eggs - not much, but fair.
6) text - prints all functions with definitions, tab prints names of functions, right arrow tries to predict your next step.
Clear cleans console.
7) quit / exit - finish

Every command is saved. Use this with up-down-arrows.
Code is supposed for every test show its result, also throwing exceptions correctly.
To launch - launch Console.py
Screenshots with proofs this not to be a decoy are in Photoes folder.