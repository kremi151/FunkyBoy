# Unit tests for FunkyBoy

![Test](https://github.com/kremi151/FunkyBoy/workflows/Test/badge.svg)

## Run the tests manually

1. Checkout submodules:

```
git submodule update [--init] --recursive
```

2. Download latest [Mooneye ROM tests](https://gekkio.fi/files/mooneye-gb/latest/mooneye-gb_hwtests.zip)

3. Extract the ZIP content to `test/mooneye-test-roms`

4. Compile and run:

```
mkdir -p cmake-test && cd cmake-test
cmake .. && make
./fb_tests --mooneye
```
