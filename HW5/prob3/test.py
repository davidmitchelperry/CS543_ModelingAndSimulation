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
	ys = []
	for j in range(30):
		output = subprocess.Popen(["p1", str(seeds[j]), str(i)], stdout=subprocess.PIPE).communicate()[0]
		split = output.split('\n')
		mean = split[30].split()[3]
		values.insert(j-1, float(mean))
		y_total = 0;
		for l in range(8):
			y_total += float(split[17 + l].split()[3])

		ys.insert(j-1, y_total)

		res += mean
		res += "\n"
		total += float(mean)

	res += "average: "
	res += str(total/30.0)
	res += "\n"

	
	not_fin = 1
	n = 0	
#	while not_fin == 1:
	x_bar = 0
	for val in values:
		x_bar += val

	x_bar = x_bar/30 + n
	y_bar = 1

	m = 0
	holder = 0
	while m < len(values):
		holder += ((values[m] - total/30) * (ys[m] - 8))
		m += 1
	cov = holder/30
	m = 0
	holder = 0
	y_avg = 0
	while m < len(ys):
		y_avg += ys[m]
		holder += pow((ys[m] - 8) , 2)
		m += 1

	y_avg = y_avg / 30

	var = holder / 29 
	
	c_star = -1 * (cov/var)

	res += "cov: "
	res += str(c_star * (y_avg - 8))
	res += "\n"

	est = cov + (total/30)

	std_dev = 0
	n = 0
	for val in values:
		std_dev += pow(val - (est), 2)
		std_dev = std_dev/(30 + n)
		std_dev = math.sqrt(std_dev)

	res += "std_dev: "
	res += str(std_dev)
	res += "\n"

	res += "upper: "
	res += str(total/30 + std_dev)
	res += "\n"

	res += "upper: "
	res += str(total/30 - std_dev)
	res += "\n"

				

#		if 1.96 * (std_dev/ math.sqrt(30 + n)) > .1:
#			print("NOT GOOD ENOUGH")
#			n += 1
#			output = subprocess.Popen(["p1", str(seeds[j]), str(i)], stdout=subprocess.PIPE).communicate()[0]
#			split = output.split('\n')
#			mean = split[30].split()[3]
#			total += float(mean)
#			values.insert(30 + (n-1), float(mean))
#		else:
#			not_fin = 0
#
#
#		res += "std_dev: "
#		res += str(std_dev)
#		res += "\n"

	
			
		
		
			


	
	

	i+= 1

f = open("res.txt", "w")
f.write(res)


		
		


