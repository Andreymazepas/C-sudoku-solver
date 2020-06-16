#Trabalho 4 da disciplina Teoria e Aplicação de Grafos 2019/1

Alunos:	Andrey Emmanuel Matrosov Mazépas	16/0112362
	Mateus Luis Oliveira			18/0112490

O sudoku está representado como um grafo de 81 vértices, 
onde cada vértice tem como adjacentes os vértices da sua linha, 
coluna e quadrado 3x3. E as cores como sendo os números de 1 a 9, 
em que não se pode colorir vértices adjacentes com a mesma cor.

Para execução a partir desta pasta:
	>$make
	>$./p4

As instruções para utilização do programa serão exibidas na tela.
Caso não conste a biblioteca ncurses no sistema, instalar com o seguinte comando:
	$sudo apt-get install libncurses5-dev libncursesw5-dev
