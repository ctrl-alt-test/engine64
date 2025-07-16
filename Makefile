debug:
	mkdir -p build/debug
	cd build/debug && cmake ../.. -DCMAKE_BUILD_TYPE=debug && make -j4 $(TARGET)

release:
	mkdir -p build/release
	cd build/release && cmake ../.. -DCMAKE_BUILD_TYPE=release && make -j4 $(TARGET)

clean:
	rm -Rvf build
