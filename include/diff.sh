for i in *.h
do
echo $i
diff $i ~/test/lpj_script/include/$i
echo ==============================================================
done
