import subprocess
import re
import matplotlib.pyplot as plt
import sys

def rodar(arq,rep,size,qnt):
	programa="./intset-ll"
	argumento=["-i ","-r "]
	aux=1
	while(aux<=rep):			#Quantidade de repeticoes
		rep0=rep
		size0=size
		qnt0=qnt
		while(qnt0!=0):		#Variados tamanhos
				teste=subprocess.run([programa, argumento[0]+str(size0),argumento[1]+str(size0)], stdout=subprocess.PIPE)
				linhas = teste.stdout.splitlines()
				salvarRAW(arq,size0,aux,linhas)
				qnt0-=1
				size0=size0*2
		aux+=1
		
	return

def procura(txt,arq):
	for line in arq:
		line=str(line)
		if(txt in line):		#Procura a variavel
			line=enumera(line)
			return int(line)
			

def graf(arq,x,y,intCon):
	plt.rcParams['figure.figsize'] = (11,7)
	plt.plot(x, y, color = 'navy' )
	plt.plot( x, y, 'ro')
	menos=[]
	mais=[]
	contador=0
	for valor in y:
		menos.append(valor-intCon[contador])
		mais.append(valor+intCon[contador])
		contador+=1
		
	plt.fill_between(x, menos, mais, color='blue', alpha=0.1)
	plt.title("Grafico")
	plt.grid(True)
	plt.xlabel("Tamanho")
	plt.ylabel("Velocidade")
	plt.savefig(arq+"/grafico.pdf")
	plt.show()

def salvarRAW(diretorio,size,num,text):
	arq=open(diretorio+"/teste"+str(size)+"_"+str(num),"w")	#Nome do arquivo
	for line in text:
		arq.write(str(line)+"\n")				#Salva as linhas
	return

def abrirRAW(txt,diretorio,indice):
	listaAux=[]
	lista=[]
	listaTam=[]
	rep=indice[0]
	tamanho=indice[1]
	aumentar=indice[2]
	contador=1
	contador2=1
	while(contador<aumentar):
		contador2=1
		listaAux=[]
		while(contador2<=rep):
			arq=open(diretorio+"/teste"+str(tamanho)+"_"+str(contador2),"r")
			aux=arq.read()
			aux=aux.splitlines()
			listaAux.append(procura(txt,aux))
			contador2+=1
		listaTam.append(tamanho)
		lista.append(listaAux)		
		tamanho=tamanho*2
		contador+=1
	return listaTam,lista
			
	
def mkdir(arq):				#Cria o diretorio
	aux=1
	teste=subprocess.run(["mkdir",arq],stdout=subprocess.PIPE)
	return arq
	'''if(teste.returncode==0):
		return arq
	else:
		while(teste.returncode!=0):
			arq0=arq+str(aux)
			teste=subprocess.run(["mkdir",arq0],stdout=subprocess.PIPE)
			aux+=1
		return arq0	'''

def criaIndice(arq,rep,tamanho,aumentar):
	arquivo=open(arq+"/indice.txt","w")			#Salva os tamanhos e quantidade de testes
	arquivo.write("["+str(rep)+","+str(tamanho)+","+str(aumentar)+']')
	return

def abrirIndice(arq):
	arquivo=open(arq+"/indice.txt","r")			#Resgata os tamanhos e quantidade de testes
	lista=arquivo.read()
	lista=eval(lista)
	return lista

def desvioPadrao(lista):		#Calcula desvio Padrao
	x=(sum(lista))/len(lista)
	contador=0
	total=0
	while(contador<len(lista)):
		total=((lista[contador]-x)**2)+total
		contador+=1
	total=total/len(lista)
	total=total**(1/2)
	return total

def IntConfianca(lista):
	Z=1.96				#Representa o valor da distribuição normal padrão para o nível de confiança de 95%
	desvio=desvioPadrao(lista)
	return(Z*(desvio/(len(lista)**(1/2))))
	
def mediaLista(listas):
	medias=[]
	for lista in listas:
		medias.append((sum(lista))/len(lista))
	return medias

def enumera(lista):					#Remove os textos,deixando apenas os numeros
	retornar=[]
	if(type(lista) is list):
		for line in lista:
			valor=''
			line=str(line)
			line=re.sub(r'#txs          : ','',line)
			line=re.sub(r' / s','',line)
			line=list(line)
			x=0
			while(line[x]!='('):
				x+=1
			x+=1
			while(line[x]!=')'):
				valor=valor+line[x]
				x+=1
			retornar.append(eval(valor))
	else:
		valor=''
		lista=str(lista)
		lista=re.sub(r'#txs          : ','',lista)
		lista=re.sub(r' / s','',lista)
		lista=list(lista)
		x=0
		while(lista[x]!='('):
			x+=1
		x+=1
		while(lista[x]!=')'):
			valor=valor+lista[x]
			x+=1
		retornar=eval(valor)
		
	return retornar

def main(arg):
	rep=10					#Quantas vezes ira repetir
	tamanho=128				#Tamanho inicial
	aumentar=8				#Quantas vezes o tamanho ira aumentar
	arq="/home/lucas/Desktop/teste"
	txt='#txs          :'
	arq=mkdir(arq)
	x=[]
	final=[]
	intervalo=[]
	if(len(arg)==1 or len(arg)>2):
		print("Modo de uso: python3 bash.py <argumento>\nDigite <python3 bash.py h> para lista de comandos")
		return
	
	elif(arg[1]=='h'):
		print("Comandos:\ng --> Plota o grafico com os dados prontos\na --> Roda o programa e produz o grafico em seguida\nr --> Apenas roda o programa")
		return
		
	elif(arg[1]=='a'):
		criaIndice(arq,rep,tamanho,aumentar)
		rodar(arq,rep,tamanho,aumentar)
		x,y=abrirRAW(txt,arq,[rep,tamanho,aumentar])
		medias=mediaLista(y)
		for lista in y:
			intervalo.append(IntConfianca(lista))
		graf(arq,x,medias,intervalo)
		return
	
	elif(arg[1]=='r'):
		criaIndice(arq,rep,tamanho,aumentar)
		rodar(arq,rep,tamanho,aumentar)
		return		
	
	elif(arg[1]=='g'):
		indice=abrirIndice(arq)
		x,y=abrirRAW(txt,arq,indice)
		medias=mediaLista(y)
		for lista in y:
			intervalo.append(IntConfianca(lista))
		graf(arq,x,medias,intervalo)
		return
		
	else:
		print("Comando errado\nDigite <python3 bash.py h> para lista de comandos")
		return
	
	

main(sys.argv)
#criaIndice("/home/lucas/Desktop/teste",10,128,8)
