import subprocess
import math

i = 2
res =""
seeds = []
for k in range(100):
	seeds.insert(k-1, k)

while i <= 11:
	res += "for arr time: "
	res += str(i)
	res += "\n"
	total = 0
	values = []
	for j in range(30):
		output = subprocess.Popen(["p1", str(seeds[j]), str(i)], stdout=subprocess.PIPE).communicate()[0]
		split = output.split('\n')
		mean = split[30].split()[3]
		values.insert(j-1, float(mean))
		res += mean
		res += "\n"
		total += float(mean)

	res += "average: "
	res += str(total/30.0)
	res += "\n"

	
	not_fin = 1
	n = 0	
	while not_fin == 1:
		std_dev = 0
		for val in values:
			std_dev += pow(val - (total/(30 + n)), 2)
			std_dev = std_dev/(30 + n)
			std_dev = math.sqrt(std_dev)

		if 1.96 * (std_dev/ math.sqrt(30 + n)) > .1:
			print("NOT GOOD ENOUGH")
			n += 1
			output = subprocess.Popen(["p1", str(seeds[j]), str(i)], stdout=subprocess.PIPE).communicate()[0]
			split = output.split('\n')
			mean = split[30].split()[3]
			total += float(mean)
			values.insert(30 + (n-1), float(mean))
		else:
			not_fin = 0


		res += "std_dev: "
		res += str(std_dev)
		res += "\n"

	
			
		
		
			


	
	

	i+= 1

f = open("res.txt", "w")
f.write(res)


		
		


