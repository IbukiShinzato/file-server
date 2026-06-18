# file-server
ホームディレクトリ上でのファイル操作（取得、作成、削除）ができるサーバとクライアント

## 実行結果1
```
$ make run-client
> GET<test.txt>
NOT FOUND

> PUT<test.txt><testfile>
CREATED

> GET<test.txt>
FILE(8): testfile

> DEL<test.txt>
DELETED

> GET<test.txt>
NOT FOUND
```

## 実行結果2
```
$ make test-files
$ make run-test-client
[test2.txt]
FILE(33): jeUuw7z0xf02uIical04lA75QV3CXhwr

[test1.txt]
FILE(33): NzfQGeVjN6wja9p6AAL3FIxVBNkiWWle

[test4.txt]
FILE(33): KX7R6cYWX5FfMhu4uwmAegB3UWjydHFG

[test5.txt]
FILE(33): mfNXenvrvc5Mo2AeNAumgCWMkKZryFKi

[test3.txt]
FILE(33): Rqc4rFdXSEHHrpOP5BXI9U2CBuxZM7ln

[test6.txt]
FILE(33): NLMFSBv3Pdyum6yJsFu5zJ3kwfYxp4pS

[test8.txt]
FILE(33): Z8oGNlD5tJ3Ysz90xl3NeOZ9OKmmEMhw

[test7.txt]
FILE(33): QtMJn8Gae2mfoD63ybQNaTrYSplx2Lso

[test11.txt]
FILE(33): qe3VpLLtqCAvYs15647G2bkDB1Ve45jW

[test13.txt]
FILE(33): rP9rusigFZ31CZFx87FhnPyaYQXddAWh

[test15.txt]
FILE(33): XeT9Vr4fRZlBaS7QGkJM8Wcd3gpr08Gv

[test9.txt]
FILE(33): McyQP3abrZMlUfE4FfyNTRchmkb1PcGb

[test12.txt]
FILE(33): fYy8r97hcAg1nWMqzuzK08U1PNmYQ4hq

[test14.txt]
FILE(33): oguWEy0yjdZ4nRUszrRSMrc2M94lgka5

[test16.txt]
FILE(33): 19FC8OgienzPycjlSBMxKBy4uhYROETF

[test10.txt]
FILE(33): jCCoda6h6UMHhMvSRL7IfCtUXaTNTUPr
```
