cd /home/ray/riscv-crypto;
source bin/conf.sh;
set -e;
cd /home/ray/rvkrypto-fips;
make clean;
echo "Test made at $(date) for RV32 arch." > output.txt;
make -j14 -f rv32.mk >> output.txt ;
sed '2,33d' output.txt > output_temp.txt && mv output_temp.txt output.txt;
echo "Test made at $(date) for RV32 arch." > instrctions.txt;
/home/ray/riscv-crypto/build/riscv64-unknown-elf/bin/spike -l --isa=rv32imac_zkn_zks_zkr /home/ray/riscv-crypto/build/riscv64-unknown-elf/riscv32-unknown-elf/bin/pk ./xtest 2>>\
/home/ray/rvkrypto-fips/instrctions.txt;
echo "Test made at $(date) for RV32 arch finished successfully."
echo "Check output.txt for results."