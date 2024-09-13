# Pseudo

Read this in other languages: [English](README.md), [日本語](README.ja.md)

* A SiTCP pseudo device (Python) for connecting to the sample program and sending data.
	* Requires the [sitcpy library](https://github.com/BeeBeansTechnologies/sitcpy) to run.

## Operating environment
* Operation has been confirmed with Python 3.11.4.

## How to use
* Install `sitcpy` with the `pip` command.
	* You can also install `sitcpy` using the included `requirements.txt`.

```
pip install sitcpy
```

* Run pseudo program.
```
python .\SiDaqPseudo.py
```

* Option --port: You can specify the TCP port number. (Default: 24242)
* Option --header: You can specify the byte length of the header to be added to the data to be sent. (Default: 2)
	* The header stores the byte length of the data frame to be sent.
	* If the data frame length does not fit the specified header length, the header will not be added.
* Option --unit: Specifies the unit of data to be sent at one time. (Default: 4096)
	* A data frame with a length of `8 bytes * the specified data unit` is created in one sending process.
* Listening starts at 0.0.0.0.
	* The UDP port is fixed at 4660.
* When connected with `SiDaqRun`, data sending starts automatically.

### Example of startup command

```
python .\SiDaqPseudo.py --port 24 --header 0 --unit 1
```

* The pseudo program starts with TCP port number 24, no data length header, and data frame length: 1 * 8 = 8 bytes.