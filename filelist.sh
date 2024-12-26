export LASTPATH=''
export RESOURCE_FILE='hannah_files.mk'
rm hannah_files.mk
#For each file in the resources tree, create an Automake variable for the purposes of installing
for i in `bfs resources -type f -print` 
do
	export FULLPATH=$i
	export CURRPATH=`dirname $i`
	if [ "$LASTPATH" != "$CURRPATH" ]; then
		if [ "$OUTPATH" != "" ]; then
			echo $LASTFILE >> ${RESOURCE_FILE}
			echo ${OUTPATH}dir='$(DATA_INSTALL_DIR)/'${LASTPATH} >> ${RESOURCE_FILE}
			echo ${OUTPATH}_DATA="\$(${OUTPATH}_files)" >> ${RESOURCE_FILE}
		fi		
		export LASTPATH=$CURRPATH
		export OUTPATH=${CURRPATH//\//_}
		echo ${OUTPATH}_files
		echo ${OUTPATH}_files ' = \' >> ${RESOURCE_FILE}
	else
		echo $LASTFILE >> ${RESOURCE_FILE} ' \'
	fi
	export LASTFILE=$i

done
echo $LASTFILE >> ${RESOURCE_FILE}
echo ${OUTPATH}dir='$(DATA_INSTALL_DIR)/'${LASTPATH} >> ${RESOURCE_FILE}
echo ${OUTPATH}_DATA="\$(${OUTPATH}_files)" >> ${RESOURCE_FILE}
