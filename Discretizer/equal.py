#Borja :)

import sys
import argparse	

if __name__=="__main__":

	parser = argparse.ArgumentParser()
	parser.add_argument("-f","--file",help="Input File")
	parser.add_argument("-o","--output",help="Output File")
	parser.add_argument("-k","--k",help="K argument",type=int)
	parser.add_argument("-nf","--features",help="FeaturesSize",type=int)
	parser.add_argument("-c","--clasS",help="Number of the class type",type=int)
	args = parser.parse_args()

	featuresValuesMax = []
	featuresValuesMin = []	

	if args.file and args.output and args.k and args.features and args.clasS:
		print "All data avaible"
	else:	
		print "Use --help for more info"
		sys.exit()
	contLine = 0
	featureCont = 0

	with open(args.output,"w") as outputFile:
		with open(args.file,"r") as inputFile:
			for line in inputFile:
				print contLine
				if contLine == 0:
					outputFile.write(line)
				else:
					for feature in line.split(","):
						if featureCont != (args.clasS-1):
							Ffeature = float(feature)
							if contLine == 1:
								featuresValuesMax.append(Ffeature)
								featuresValuesMin.append(Ffeature)
							else:
								if args.clasS == 1:
									resta = 1
								else:
									resta = 0
								if featuresValuesMax[featureCont-resta] < Ffeature:
									featuresValuesMax[featureCont-resta] = Ffeature
								if featuresValuesMin[featureCont-resta] > Ffeature:
									featuresValuesMin[featureCont-resta] = Ffeature
						featureCont = featureCont+1
					featureCont = 0
				contLine = contLine + 1

		w = list((i-j)/args.k for (i,j) in zip(featuresValuesMax,featuresValuesMin))
		print len(w)
		contLine = 0	
		with open(args.file,"r") as inputFile:
			for line in inputFile:
				print contLine
				if contLine > 0:
					lineaInteres = []
					for feature in line.split(","):
						if featureCont != (args.clasS-1):
							Ffeature = float(feature)
							if args.clasS == 1:
								resta = 1
							else:
								resta = 0
							if (w[featureCont-resta] != float(0)):
								lineaInteres.append(str(int((Ffeature-featuresValuesMin[featureCont-resta])/w[featureCont-resta])))
							else:
								lineaInteres.append(str(Ffeature))
						else:
							lineaInteres.append(str(feature));						
						featureCont = featureCont+1

					featureCont = 0
					write = ",".join(lineaInteres)
					write = write.strip()+"\n"
					outputFile.write(write)
				contLine = contLine + 1					
	
			
	print "Finish properlly"
