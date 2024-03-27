bash compile.sh
echo "starting with escherichia"
bash executeTest.sh files/escherichia.faa scalability/info/escheNotAll2.info scalability/grh/escheNotAll3.grh
echo "starting with salmonella"
bash executeTest.sh files/salmonella.faa scalability/info/salmoNotAll2.info scalability/grh/salmoNotAll3.grh
echo "starting with mycoplasma"
bash executeTest.sh files/mycoplasma.faa scalability/info/mycoNotAll2.info scalability/grh/mycoNotAll3.grh
echo "starting with xanthomonas"
bash executeTest.sh files/xanthomonas.faa scalability/info/xanthoNotAll2.info scalability/grh/xanthoNotAll3.grh

# echo "starting with escherichia*2"
# bash executeTest.sh files/escherichia_double_genes.faa scalability/info/doubleEschDell2.info scalability/grh/doubleEschDell2.grh
