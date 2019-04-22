#!/usr/bin/python

import os

currAvg = -1
besti = -1
bestj = -1

i = 1 
j = 1 

while i < 300:

	j = 1		
	while j < 300:

		command = "./ex1 " + str(i) + " " + str(j) + " > out.txt"
		print(command)

		os.system(command)

		with open("out.txt") as f:
			content = f.readlines()

		if len(content) < 15:
			
			AtoBTime = float(content[5].split()[6])
			BtoATime = float(content[6].split()[6])

			avg = (AtoBTime + BtoATime) / 2

			if currAvg == -1 or avg < currAvg:
				currAvg = avg
				besti = i
				bestj = j

		j += 1
	i += 1

f = open('res.txt', 'w')

result = "Best i: " + str(besti) + " Best j: " +   str(bestj) + "\n"
f.write(result)
f.close()









