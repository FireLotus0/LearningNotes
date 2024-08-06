1. 先查看之前有没有配置过ssh
```cpp
ls -al ~/.ssh
```

2. 如果有配置，查看配置的用户名和邮箱
```cpp
git config user.name
git config user.email
```

3. 如果没有配置，进行配置
```cpp
git config -global user.name "name"
git config -global user.email "email"
```

4. 生成密钥
```cpp
ssh-keygen -t rsa -C "配置的邮箱"
```

5. 接着按下三个回车，配置没有密码，否则输入密码

6. 生成两个文件，id_rsa文件（私钥）， id_rsa.pub(公钥)

7. 将公钥配置到github即可

