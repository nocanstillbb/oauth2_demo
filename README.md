# 登录和刷新token

auth server提供两个api 

+ login : 用于获取access_token 和 refresh_token
+ refresh :  使用refresh_token 更新access_token

这两个接口都只需要Body中有json数据就行



## `login`

post

```json
{
    "username": "user",
    "password": "password"
}
```

return 

```json
{
    "access_token": "eyJhbGciOiJIUzI1NiJ9.eyJleHAiOjE3MzAwMjk3MDIsImlzcyI6ImF1dGhfc2VydmVyIiwicm9sZSI6InVzZXJfcm9sZSIsInN1YiI6InVzZXIifQ.SLnN0ei-PrzXciQwLwQWFXLsycIRoQq9IFoEOknQj_0",
    "expires_in": 900,
    "refresh_token": "eyJhbGciOiJIUzI1NiJ9.eyJleHAiOjE3MzA2MzM2MDIsImlzcyI6ImF1dGhfc2VydmVyIiwicm9sZSI6InVzZXJfcm9sZSIsInN1YiI6InVzZXIifQ.GOsmnbBEynNy8VMC_XWBzjuKv5o0NfuQYjRn2YLp-Fs",
    "token_type": "Bearer"
}
```



## `refresh`

post

```json
{
    "refresh_token": "eyJhbGciOiJIUzI1NiJ9.eyJleHAiOjE3MzA2MzM2MDIsImlzcyI6ImF1dGhfc2VydmVyIiwicm9sZSI6InVzZXJfcm9sZSIsInN1YiI6InVzZXIifQ.GOsmnbBEynNy8VMC_XWBzjuKv5o0NfuQYjRn2YLp-Fs"
}
```

return

```json
{
    "access_token": "eyJhbGciOiJIUzI1NiJ9.eyJleHAiOjE3MzAwMjk3MzksImlzcyI6ImF1dGhfc2VydmVyIiwicm9sZSI6InVzZXJfcm9sZSIsInN1YiI6InVzZXIifQ.hirAgzD2_-OmPML_mrrpofmit36Bt0taWY1oyDC2WlI",
    "expires_in": 900
}
```





# 使用token 访问 rest_server

在headers中添加一对key value

key : `Authorization`

value :  `Bearer eyJhbGciOiJIUzI1NiJ9.eyJleHAiOjE3MzAwMjk3MzksImlzcyI6ImF1dGhfc2VydmVyIiwicm9sZSI6InVzZXJfcm9sZSIsInN1YiI6InVzZXIifQ.hirAgzD2_-OmPML_mrrpofmit36Bt0taWY1oyDC2WlI`



```c
CURL *curl;
CURLcode res;
curl = curl_easy_init();
if(curl) {
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8081/service");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Authorization: Bearer eyJhbGciOiJIUzI1NiJ9.eyJleHAiOjE3MzAwMjk3MzksImlzcyI6ImF1dGhfc2VydmVyIiwicm9sZSI6InVzZXJfcm9sZSIsInN1YiI6InVzZXIifQ.hirAgzD2_-OmPML_mrrpofmit36Bt0taWY1oyDC2WlI");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  res = curl_easy_perform(curl);
  curl_slist_free_all(headers);
}
curl_easy_cleanup(curl);
```



value中`bearer ` (有一个空格) 是固定的 后面接access_token

