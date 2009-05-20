TARGET = gpu_test.sh
BIN = ../../../bin/


all:
	-rm -rf $(TARGET)
	if [ $$ARCH_PLATFORM == "imx51stack" ]; then cp mx51_gpu_test.sh $(TARGET); fi
	if [ $$ARCH_PLATFORM == "imx35stack" ]; then cp mx35_gpu_test.sh $(TARGET); fi

install:
	ln -f $(TARGET) $(BIN)

clean:
	-rm -rf $(TARGET)

