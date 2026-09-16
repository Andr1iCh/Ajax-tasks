Послідовність дій:

Ініціалізував локальний репозиторій із головною гілкою main:
git init -b main

Створив файл конфігурації для ігнорування небажаних файлів:
cat >> .gitignore

Налаштував глобальні дані користувача для підпису коммітів:
git config --global user.name ""
git config --global user.email ""

Додав усі підготовлені локальні файли до індексу:
git add .

Зафіксував локальні файли першим коммітом:
git commit -m "add local files"

Прив'язав віддалений репозиторій GitHub як origin:
git remote add origin https://github.com/Andr1iCh/Ajax-tasks.git

Перевірив коректність прив'язаної адреси віддаленого репозиторію:
git remote -v

Встановив створення звичайного комміту злиття як стратегію за замовчуванням:
git config pull.rebase false

Стягнув віддалені зміни та об'єднав дві незалежні гілки з окремими історіями:
git pull origin main --allow-unrelated-histories

Відправив об'єднану історію у віддалений репозиторій на GitHub:
git push -u origin main
  
