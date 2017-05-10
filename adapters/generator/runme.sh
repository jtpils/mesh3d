fn=$1
pt=$2
cat AdapterTemplate.h.in | sed -e "s/%function%/$fn/" -e "s/%param%/$pt/" | tee $fn.h
cat AdapterTemplate.cxx.in | sed -e "s/%fn%/$fn/" -e "s/%param%/$pt/" | tee $fn.cxx
