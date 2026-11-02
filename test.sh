#!/usr/bin/bash

BIN="./ft_malcolm"

passed=0
failed=0

run_test() {
    local description="$1"
    local expected="$2"
    shift 2

    "$BIN" "$@" > /dev/null 2>&1
    local actual=$?

    if [[ "$actual" -eq "$expected" ]]; then
        echo -e "[PASS] $description"
        ((passed++))
    else
        echo -e "[FAIL] $description (expected: $expected, got: $actual)"
    fi
}

echo "ft_malcolm parsing tester"

run_test "valid ipv4 and mac address" 0 \
    192.168.1.10 AA:BB:CC:DD:EE:FF \
    192.168.1.20 11:22:33:44:55:66

run_test "lowercase mac address" 0 \
    192.168.1.10 aa:bb:cc:dd:ee:ff \
    192.168.1.20 aa:bb:cc:dd:ee:11

run_test "broadcast mac address" 0 \
    192.168.1.10 FF:FF:FF:FF:FF:FF \
    192.168.1.20 00:00:00:00:00:00

run_test "invalid ipv4 octet" 1 \
    192.168.1.256 AA:BB:CC:DD:EE:FF \
    192.168.1.20 11:22:33:44:55:66

run_test "invalid ipv4 characters" 1 \
    192.168.1.abc AA:BB:CC:DD:EE:FF \
    192.168.1.20 11:22:33:44:55:66

run_test "empty field" 1 \
    "" AA:BB:CC:DD:EE:FF \
    192.168.1.20 11:22:33:44:55:66

run_test "invalid mac" 1 \
    192.168.1.10 GG:BB:CC:DD:EE:FF \
    192.168.1.20 11:22:33:44:55:66

run_test "invalid mac separator" 1 \
    192.168.1.10 AA-BB-CC:DD:EE:FF \
    192.168.1.20 11:22:33:44:55:66

run_test "too short mac address" 1 \
    192.168.1.10 AA:BB:CC:DD:EE \
    192.168.1.20 11:22:33:44:55:66

run_test "too long mac address" 1 \
    192.168.1.10 AA:BB:CC:DD:EE:FF \
    192.168.1.20 11:22:33:44:55:66:77

run_test "missing mac octet digit" 1 \
    192.168.1.10 AA:BB:C:DD:EE:FF \
    192.168.1.20 11:22:33:44:55:66


run_test "Extra argument" 1 \
    192.168.1.10 AA:BB:CC:DD:EE:FF \
    192.168.1.20 11:22:33:44:55:66 extra

run_test "Verbose option" 0 \
    -v 192.168.1.10 AA:BB:CC:DD:EE:FF \
    192.168.1.20 11:22:33:44:55:66

echo "SUMMARY"
echo -e "PASSED: $passed"
echo -e "FAILED: $failed"

if [[ "$failed" -gt 0 ]]; then
    exit 1
fi

exit 0














