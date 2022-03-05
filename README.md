# Умная розетка и умное реле grib technology
Розетка и реле grib technology - устройства системы умного дома на основе API Telegram.

![DSC06524](https://user-images.githubusercontent.com/84660518/156652191-d5a9511f-c55e-4593-9be8-8c6b3b57945d.jpg)
![DSC06530](https://user-images.githubusercontent.com/84660518/156652329-725f3acb-2d90-4d71-899d-be960b87dbc5.jpg)


Приобрести розетку можно на [Wildberries](https://www.wildberries.ru/catalog/66337720/detail.aspx?targetUrl=SN)

## Содержание
- [Умная розетка и умное реле grib technology](#socket)
- [Подробнее о системе и коде](#code)
- [Плата и основные компоненты](#components)
- [Схема и работа печатной платы](#scheme)
- [Ответственность](#responsibility)
- [Контакты](#contacts)


<a id="socket"></a>
# Умная розетка и умное реле grib technology

Розетка и реле управляются с помощью c помощью шлюза — устройства [Miss You Even When We're Together](https://github.com/grib-technology/grib_myewwt_clock) (MYEWWT) а также с помощью [выключателя](https://github.com/grib-technology/grib_switch).

MYEWWT взаимодействует c API Telegram, позволяя пользователю отправлять команды и предоставляет на них ответ в Telegram-боте, заренее созданном пользователем. 

MYEWWT подключается к Интернету через WI-Fi и взаимодействует с Telegram ботом, получая команды от него, обрабатывает их и управляет реле и розеткой с помощью технологии [ESP-NOW.](https://www.espressif.com/en/products/software/esp-now/overview)

Возможности устройства MYEWWT:

* соединение с точкой доступа Wi-Fi и выход в интернет,
* обмен данными с другими устройствами умного дома с применением технологии [ESP-NOW](https://www.espressif.com/en/products/software/esp-now/overview),
* контроль температуры и влажности воздуха в помещении,
* возможность смены цвета сегментов часов на любой из диапазона RGB.


<a id="code"></a>
# Подробнее о системе и коде
Здесь и далее находится описание реле и розетки grib technology. Информацию о системе умного дома grib technology, особенностях кода прошивки вы можете найти [здесь](https://github.com/grib-technology/grib_myewwt_clock/blob/main/ABOUT.md).

<a id="components"></a>
# Плата и основные компоненты

![Безымянный2](https://user-images.githubusercontent.com/84660518/156656077-4f18c050-68e5-47ef-8cb8-3649ebe82644.png)

Основные компоненты:
 * микроконтроллер Espressif ESP-12F,
 * реле Т73 5VDC,
 * блок питания,
 * кнопка.

<a id="scheme"></a>
# Схема и работа печатной платы
![Schematic_relay v2_2022-03-04](https://user-images.githubusercontent.com/84660518/156656594-a3edc0f4-5103-4742-ace6-3b302aa78984.png)


На плату подаётся питание 5 В от блока питания, подключенного через изоляционную прокладку. Так как микроконтроллеру ESP-12F требуется питание 3.3 В, предусмотрен блок стабилизации напряжения — непосредственно сам стабилизатор AMS1117-3.3, а также входные и выходные сглаживающие конденсаторы для обеспечения стабильности работы платы. 

Для корректной работы реле, в отличие от ESP-12f, требуется 5 В, поэтому на плате предусмотрен транзистор MMBT2222A, позволяющий управлять обмоткой реле. 

Также в схему включена тактовая кнопка для удобства аналогового управления а также поддержки возможности управления реле или розеткой при отсутствии соединения с интернетом.


<a id="responsibility"></a>
# Ответственность

В прошивке розетки и реле используются библиотеки из проектов с открытым исходным кодом на основании лицензий, закреплёнными за авторами библиотек. Таким образом, работоспособность библиотек не может быть гарантирована. Также нет никаких гарантий, что устройство будет работать с вашей электросетью, и вы не получите ущерба во время эксплуатации устройства.

[Лицензия GNU General Public License v3.0](https://github.com/grib-technology/grib_socket_and_relay/blob/main/LICENSE.md)

<a id="contacts"></a>
# Контакты

www.grib-tech.ru

Приобрести розетку можно на [Wildberries](https://www.wildberries.ru/catalog/66337720/detail.aspx?targetUrl=SN)

[Telegram-канал](t.me/grib_tech) 

[Instagram](instagram.com/grib.tech)

Для связи:

[Бот-помощник](t.me/grib_tech_bot) 

[Чат с разработчиком](t.me/rafflezy)

