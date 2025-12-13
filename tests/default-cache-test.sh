./4-parallel-clients.sh --port 8080 --url http://xcal1.vodafone.co.uk/5MB.zip --requests 100 --timeout 40



./2-concurrent-batches.sh --port 8080 --url http://xcal1.vodafone.co.uk/5MB.zip --requests 50 --timeout 40 --concurrency 20
