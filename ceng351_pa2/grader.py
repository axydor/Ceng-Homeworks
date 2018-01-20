# -*- coding: utf-8 -*-
import sys
import os

funcTestCommandPrefix = "diff --suppress-common-lines --ignore-blank-lines "
javaCompileCommand = "javac *.java"

javaRunCommand = "java CengCoinExchange "

def checkOutput(fileName):

	global outputFolder

	funcTestCommand = funcTestCommandPrefix + "Outputs/" + fileName + " " + outputFolder + "Answer_" + fileName + " > ./Differences/difference_" + fileName

	if (os.system(funcTestCommand) > 0): 
		return -1
	else:
		# Remove empty difference text 
		os.system("rm ./Differences/difference_" + fileName)

		return 1

if __name__ == '__main__':
	global outputFolder

	# You could use ${PWD} for OutputFolder if the Answers are in the same folder.
	# If not, you could do something like ./ExpectedOutputs

	if len(sys.argv) < 3:
		sys.exit('Usage: %s produce-only (1 or 0) OutputFolder')

	shouldNotDiff = int(sys.argv[1])

	outputFolder = sys.argv[2]

	print 

	allTests = []

	totalPoints = 0

	easyCasesWithoutSearch = ["10Sorted_NS", "20Sorted_NS", "10Shuffled_NS", "20Shuffled_NS"]
	easyCases = ["10Sorted", "20Sorted", "10Shuffled", "20Shuffled"]
	intermediateCases = ["100Sorted", "200Sorted", "100Shuffled", "200Shuffled"]
	hardCases = ["1000Sorted", "2000Sorted", "1000Shuffled", "2000Shuffled"]

	printFileNames = easyCasesWithoutSearch + easyCases + intermediateCases + hardCases

	for fileName in printFileNames:
		with open(fileName + ".txt", "r+") as testFile:
			hashMod = int(testFile.readline().split(":")[1])
			bucketSize = int(testFile.readline().split(":")[1])

			remainingLines = testFile.readlines()

			inputName = fileName + "_in.txt"
			outputName = fileName + "_out.txt"

			with open(inputName, "w") as inputFile:
				for remainingLine in remainingLines:
					inputFile.write(remainingLine)

			grade = 0

			if fileName not in hardCases:
				grade = 5
			else:
				grade = 10

			allTests.append({"hashMod" : hashMod,
							"bucketSize" : bucketSize,
							"inputName" : inputName,
							"outputName" : outputName,
							"grade" : grade})

	if os.system(javaCompileCommand) > 0:
		print "--- Java Compilation has failed. ---"
	else:
		print "--- Java Compilation is successful. ---"
		count = 0

		for currentTest in allTests:
			hashMod = currentTest["hashMod"]
			bucketSize = currentTest["bucketSize"]

			fileNameInput = currentTest["inputName"]
			fileNameOutput = currentTest["outputName"]

			if shouldNotDiff == 1:
				fileNameOutput = "Answer_" + fileNameOutput

			grade = currentTest["grade"]

			# Note: You could also use:
			# java CengCoinExchange 8 2 dummy.txt False < inputFile.txt 
			# To emulate command line feed in.
			# cat inputFile.txt | java CengCoinExchange 8 2 dummy.txt False
			# is also the same below.

			# I've tested with both, there seemed to be no difference. If there is, let me know.

			command = "cat " + fileNameInput + " | " + javaRunCommand + str(hashMod) + " " + str(bucketSize) + " dummy.txt False " + " > Outputs/" + fileNameOutput

			os.system(command)

			if not shouldNotDiff:
				if checkOutput(fileNameOutput) == 1:
					print "--------------"
					print "-Test", count + 1, "-", fileNameOutput,"- Success"
					print "--------------"
					totalPoints += grade
				else:
					print "--------------"
					print "-Test", count + 1, "-", fileNameOutput, "- Failure"
					print "--------------"

			count += 1

	print "Grade " + str(totalPoints)
	
