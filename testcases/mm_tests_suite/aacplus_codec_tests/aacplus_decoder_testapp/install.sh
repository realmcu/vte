# Convert mp4 to adif and adts
files=`find ./testvectors/mpeg_* ./testvectors/ps-conformance ./testvectors/ct_sbr_certification/erroneous ./testvectors/ct_sbr_certification/valid -name "*.mp4" | sed -e "s/\.mp4//g"`

chmod +x ./adif2mp4
chmod +x ./refdecoder

for FILE in $files; do
    ./adif2mp4 -o adif ${FILE}.mp4 ${FILE}.adif > /dev/null 2> /dev/null
    ./adif2mp4 -o adts ${FILE}.mp4 ${FILE}.adts > /dev/null 2> /dev/null
done

# make reference vector

mkdir -p Ref
mkdir -p Ref/mpeg_aac_conformance
mkdir -p Ref/mpeg_sbr_conformance
mkdir -p Ref/ps-conformance
mkdir -p Ref/ct_sbr_certification
mkdir -p Ref/ct_sbr_certification/erroneous
mkdir -p Ref/ct_sbr_certification/transport
mkdir -p Ref/ct_sbr_certification/valid
mkdir -p aacplus_output
mkdir -p aacplus_output/mpeg_aac_conformance
mkdir -p aacplus_output/mpeg_sbr_conformance
mkdir -p aacplus_output/ps-conformance
mkdir -p aacplus_output/ct_sbr_certification
mkdir -p aacplus_output/ct_sbr_certification/erroneous
mkdir -p aacplus_output/ct_sbr_certification/transport
mkdir -p aacplus_output/ct_sbr_certification/valid

./refdecoder -T0 -C makeref_cfg > /dev/null
echo "Complited"

