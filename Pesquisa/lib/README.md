# Biblioteca VSAT

Esta biblioteca é projetada para utilização com a plataforma arduino. Para execução siga as instruções seguintes.

## Compilação/Execução

1. Baixe os arquivos [VSAT.cpp](https://github.com/VSAT-Space-Program/VSAT-M1/blob/master/Pesquisa/lib/VSAT.cpp) e [VSAT.h](https://github.com/VSAT-Space-Program/VSAT-M1/blob/master/Pesquisa/lib/VSAT.h), compacte em um VSAT.zip e insira na IDE do arduino como nova biblioteca.

2. Utilize o arquivo de teste [satellite.ino](https://github.com/VSAT-Space-Program/VSAT-M1/blob/master/Pesquisa/lib/satellite.ino) para verificar o uso das funções da bilioteca. 


## Funções

* **int TX_Lora(int data_to_transmit)** - Transmite dados através do protocolo Lora.
* **int RX_Lora()** - Recebe dados através do protocolo Lora.

### Ferramentas
* [IDE Arduino](https://www.arduino.cc/en/main/software) - A IDE arduino será utilizada para o desenvolvimento do projeto.

