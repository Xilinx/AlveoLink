#mkdir tmp
#cd tmp
#mkdir verilog
#cd ..
if [ $1 == 'clean' ]
then
    cd HiveNet_bd
    rm -r */
    rm HiveNet_bd.xpr
    cd ..
    cd ip
    rm -r HiveNet
    cd ..
    rm *.log
    rm *.jou
    rm *.xml
    rm -r xgui
    rm -r .Xil
else
    vivado -mode batch -source HiveNet_bd.tcl -tclargs $1 $2
fi