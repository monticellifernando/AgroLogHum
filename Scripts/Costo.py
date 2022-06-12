#!env python
import sys, getopt, math, os
import argparse
from datetime import datetime
def main(argv):
  parser = argparse.ArgumentParser("Mostrar cuanto pasar este mes")
  parser.add_argument('-g', '--Gastos', type=str, required=False, help="Archivo de Gastos", default='Gastos.csv')
  parser.add_argument('-f', '--Fecha', type=int, required=False, help="mes y a~no para hacer el c'alculo" )
  parser.add_argument('-D','--Debug', help="Print a lot of debugging information", action="store_true")
  parser.add_argument('-H','--Historia', help="Muestra Toooda la historia de gastos", action="store_true")
  parser.add_argument('-P','--Porcentaje', help="Porcentaje del sueldo que va para la cuota", type=float, default=0.25)
  parser.add_argument('-C','--Cuota', help="Valor de la cuota", type=float)

  args = parser.parse_args()

  m_debug = args.Debug

  print('Calculando Gasto')

  import pandas as pd
  
  Resultado = {
          'Descripcion' : [],
          'Cantidad':[]
          }
  

    

 

  Gastos = pd.read_csv ( 'Gastos.csv' )
  GastosEsteMes = Gastos


  print('-----------------')
  print()
  TotalGastos=0
  for i in GastosEsteMes['Gasto']:
      if i!=i:
          i=0
      TotalGastos+=i
      Resultado['Cantidad'].append(i)
  
  for i in GastosEsteMes['Descripcion']:
      Resultado['Descripcion'].append(i)
  #Total = IngresosEsteMes.CONICET[0] + IngresosEsteMes.UNLP[0]
  Resultado['Descripcion'].append('Total:')
  Resultado['Cantidad'].append(TotalGastos)
  Resumen = pd.DataFrame(Resultado)
  print()
  print('-----------')

  print(Resumen)
#  print(Resultado)

if __name__ == "__main__":
    main(sys.argv[1:])

