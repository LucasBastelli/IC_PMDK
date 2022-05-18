import subprocess
import re
import sys

def rodar(arq,rep,Tamanho,update,programa):
	argumento=["-i ","-r ","-u"]
	aux=1
	while(aux<=rep):			#Quantidade de repeticoes
		print("Repetição "+str(aux)+" de "+str(rep)) 
		for size0 in Tamanho:		#Variados tamanhos
				teste=subprocess.run([programa, argumento[0]+str(size0),argumento[1]+str(2*size0),argumento[2]+str(update)], stdout=subprocess.PIPE)
				linhas = teste.stdout.splitlines()
				salvarRAW(arq,size0,aux,linhas)
		aux+=1
		
	return

def procura(txt,arq):
	for line in arq:
		line=str(line)
		if(txt in line):		#Procura a variavel
			line=enumera(line)
			return int(line)
			

def graf_comparacao(arq,x,y,y1,error1,error2):
	add=0.3
	plt.yscale("log")
	Sizex=['128', '256', '512', '1024', '2048']
	plt.rcParams['figure.figsize'] = (11,7)
	bar1=numpy.arange(len(x))
	bar2=[i+add for i in bar1]
	plt.bar(bar1, y,add, yerr=error1, color='r')
	plt.bar(bar2, y1,add, yerr=error2, color='b')
	plt.xticks(bar1+add/2,Sizex)
	plt.ylabel('Velocidade')
	plt.title('Comparação PM vs RAM')
	plt.legend(labels=['RAM', 'PM'])
	plt.savefig(arq+"/graficoComparacao.pdf")
	plt.show()

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

def abrirRAW(txt,diretorio,rep,indice):
	listaAux=[]
	lista=[]
	listaTam=[]
	contador=0
	contador2=1
	contadorTam=0
	tamanho=indice[0]
	while(contador<(len(indice))):
		contador2=1
		tamanho=indice[contadorTam]
		listaAux=[]
		while(contador2<=rep):
			arq=open(diretorio+"/teste"+str(tamanho)+"_"+str(contador2),"r")
			aux=arq.read()
			aux=aux.splitlines()
			listaAux.append(procura(txt,aux))
			contador2+=1
		listaTam.append(tamanho)
		lista.append(listaAux)		
		contadorTam+=1
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

def criaIndice(arq,rep,Tamanho):
	arquivo=open(arq+"/indice.txt","w")			#Salva os tamanhos e quantidade de testes
	arquivo.write("["+str(rep)+","+str(Tamanho)+']')
	return

def abrirIndice(arq):
	arquivo=open(arq+"/indice.txt","r")		#Resgata os tamanhos e quantidade de testes
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
	tamanho=[128,256,512,1024,2048,4096,8192,16384,32768,65536]
	update=50                               #Taxa de update
	PM=0
	programa="./intset-ll"
	arq="/home-ext/LucasIC/TestesHD/test"        #Pra ssd é TestesHSssd
	
	if(PM==1):
		programa=programa+"-pm"
		arq=arq+"PM"
		
	arq=arq+"_update"+str(update)
	txt='#txs          :'
	arq=mkdir(arq)
	x=[]
	final=[]
	intervalo=[]
	if(len(arg)==1 or len(arg)>7):
		print("Modo de uso: python3 bash.py <argumento>\nDigite <python3 bash.py h> para lista de comandos")
		return
	
	elif(arg[1]=='h'):
		print("Comandos:\ng --> Plota o grafico com os dados prontos\na --> Roda o programa e produz o grafico em seguida\nr --> Apenas roda o programa")
		return
		
	elif(arg[1]=='a'):
		criaIndice(arq,rep,tamanho)
		rodar(arq,rep,tamanho,update,programa)
		x,y=abrirRAW(txt,arq,rep,tamanho)
		medias=mediaLista(y)
		for lista in y:
			intervalo.append(IntConfianca(lista))
		graf(arq,x,medias,intervalo)
		return
	
	elif(arg[1]=='r'):
		criaIndice(arq,rep,tamanho)
		rodar(arq,rep,tamanho,update,programa)
		return		
	
	elif(arg[1]=='g'):
		indice=abrirIndice(arq)
		rep=indice[0]
		indice.pop(0)
		indice=indice[0]
		x,y=abrirRAW(txt,arq,rep,indice)
		medias=mediaLista(y)
		for lista in y:
			intervalo.append(IntConfianca(lista))
		graf(arq,x,medias,intervalo)
		return
	
	elif(arg[1]=='c'):
		arq="/home/lucas/Desktop/teste_update75"
		arq2="/home/lucas/Desktop/testePM_update75"
		intervalo=[]
		intervalo2=[]
		indice=abrirIndice(arq)
		indice2=abrirIndice(arq2)
		rep=indice[0]
		indice.pop(0)
		indice=indice[0]
		rep2=indice2[0]
		indice2.pop(0)
		indice2=indice2[0]
		x,y=abrirRAW(txt,arq,rep,indice)
		x2,y2=abrirRAW(txt,arq2,rep2,indice2)
		medias=mediaLista(y)
		medias2=mediaLista(y2)
		for lista in y:
			intervalo.append(IntConfianca(lista))
		for lista in y2:
			intervalo2.append(IntConfianca(lista))
		graf_comparacao(arq,x,medias,medias2,intervalo,intervalo2)
		return
		
	else:
		print("Comando errado\nDigite <python3 bash.py h> para lista de comandos")
		return
	
	
#teste
main(sys.argv)
#criaIndice("/home/lucas/Desktop/teste",10,128,8)
