# Simple proxy tests

Usage:

```bash
./<test> --help
```

## Params

- `--port` - port that local proxy is listening on. Default: 8080.
- `--url` - url to access through proxy. Default: 'http://xcal1.vodafone.co.uk/10MB.zip'.
- `--requests` - number of requests. Default: 10.
- `--timeout` - curl timeout (see `curl --max-time`). Default: 10.
- `--concurrency` - number of parallel requests if available for test. Default: 10.

## Dependencies

Basic calculator:

```bash
sudo apt install bc
```

## Examples

```bash
./1-sequential.sh --port 9000 --url http://xcal1.vodafone.co.uk/5MB.zip --requests 10 --timeout 60
```
```bash
./2-concurrent-batches.sh --port 9000 --url http://xcal1.vodafone.co.uk/5MB.zip --requests 50 --timeout 10 --concurrency 20
```
```bash
./3-cache-invalidation.sh --port 9000 --url http://xcal1.vodafone.co.uk/5MB.zip --requests 10 --timeout 10
```
```bash
./4-parallel-clients.sh --port 9000 --url http://xcal1.vodafone.co.uk/5MB.zip --requests 500 --timeout 10
```
```bash
./5-incremental-interrupt.sh --port 9000 --url http://xcal1.vodafone.co.uk/5MB.zip --requests 10 --timeout 10
```
