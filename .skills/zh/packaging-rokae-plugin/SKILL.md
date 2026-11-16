---
name: packaging-rokae-plugin
description: >-
  制作 Rokae+ 插件包时使用。插件包可只含客户端插件、只含控制器插件，或同时包含客户端插件和控制器插件。可在 Windows 或 Linux 上打包。完成后给出插件包的绝对路径。不编译，不新建客户端插件工程。
compatibility: Requires Python 3 to run scripts/check_stage.py before packaging. Do not install Python and do not search for an interpreter. Windows runs assets/PluginPackage.bat. Linux runs assets/PluginPackage.sh. Packaging does not require a compiler.
metadata:
  version: "1.0"
---

# 制作 Rokae+ 插件包

本技能把已经编译好的客户端插件和控制器插件打包成可导入的插件包。

## 插件包内容

HMI 导入的对象为插件包 zip，包内为数据包 `client.zip` 和 `controller.zip`。可以同时包含客户端插件和控制器插件，也可以只含客户端插件或只含控制器插件。客户端插件和控制器插件没有强制依赖关系。

插件包可以放入一个或多个插件。多个插件可以各自形成独立的插件包，分别导入；也可以在同一 `client.zip` 或 `controller.zip` 内并列多个插件目录，一次导入。

### 名称与文件

目录名、插件库文件名、配置文件名、授权文件名，以及配置中的 `name`，使用同一个插件名。`xplugin` 的文件名固定为 `xplugin`，后缀与插件库相同。客户端插件名和控制器插件名可以不同。禁止为了打包把客户端插件名和控制器插件名改成同一个名字。

`win` 使用 `.dll`。`aarch64` 和 `amd64` 使用 `.so`。控制器库使用 `.so`。禁止把开发包 `include/` 放入插件包。

### 配置文件

配置示例和目录示例里的 `MyPlugin`、`OtherPlugin`、`example`、`plugin1`、`plugin2` 只代指插件名。

#### 客户端插件

客户端配置示例如下：

```json
{
  "client_plugin": {
    "name": "MyPlugin",
    "depend": [],
    "ctrlDepend": [],
    "enable": false,
    "version": "3.0.1",
    "min_hmi_version": "3.0.1",
    "custom_hmi_version": [],
    "author": "rokae",
    "description": "description"
  }
}
```

| 字段 | 必填 | 说明 |
|---|---|---|
| `name` | 是 | 插件名 |
| `depend` | 否 | 所依赖的其它客户端插件名列表。无依赖时为空数组 |
| `ctrlDepend` | 否 | 所依赖的控制器插件名列表 |
| `enable` | 是 | 导入后是否默认启用 |
| `version` | 是 | 插件版本号 |
| `min_hmi_version` | 否 | 所支持的最低 HMI（xCore）版本 |
| `custom_hmi_version` | 否 | 自定义适配版本列表 |
| `author` | 否 | 作者 |
| `description` | 否 | 描述 |

一个插件依赖另一个插件时，在依赖方配置文件的 `depend` 或 `ctrlDepend` 中写入被依赖插件的插件名，例如 `"depend": ["OtherPlugin"]`。`depend` 为客户端依赖，`ctrlDepend` 为控制器依赖。

#### 控制器插件

控制器配置在 `controller_plugin` 下放置与插件名同名的对象。对象内的 `name` 使用同一个插件名。

控制器配置示例如下：

```json
{
  "controller_plugin": {
    "example": {
      "name": "example",
      "depend": ["plugin1", "plugin2"],
      "enable": true,
      "min_controller_version": "3.0.2",
      "must_controller_version": [],
      "version": "1.0.0",
      "author": "rokae",
      "description": "description info"
    }
  }
}
```

| 字段 | 必填 | 说明 |
|---|---|---|
| `name` | 是 | 插件名 |
| `depend` | 有依赖时必填 | 所依赖的其他控制器插件名列表。无依赖时可填 `[]` |
| `enable` | 是 | 导入后是否默认启用 |
| `min_controller_version` | 否 | 插件运行所需的最低控制器版本 |
| `must_controller_version` | 否 | 允许插件运行的控制器版本列表。非空时覆盖 `min_controller_version`。未指定时可填 `[]` |
| `version` | 否 | 插件版本号 |
| `author` | 否 | 作者 |
| `description` | 否 | 描述 |

## 打包工作目录

打包工作目录为用户指定的目录。用户未指定时，打包工作目录为当前工作目录。

`client/`、`controller/`、打包脚本放在打包工作目录中。打包脚本把 `client/` 压成 `client.zip`，把 `controller/` 压成 `controller.zip`，再放入以打包工作目录名命名的 zip。

## 确认事项

打包之前，可以先读取用户给出的工程、构建目录、打包工作目录，用来发现缺少的信息。缺少的事项一次问清，然后停止。确认前禁止复制文件，禁止运行打包脚本。打包前须确认下列事项：

1. 打包工作目录。
2. 本次打包客户端插件、控制器插件，或客户端插件和控制器插件一起。AI agent 取插件库文件名作为插件名，去掉 `.dll` 或 `.so`。`xplugin.dll` 和 `xplugin.so` 不作为插件名。有多份插件库时，AI agent 按每份库文件名逐个写出插件名。没有插件库文件时，请用户给出插件名。禁止推测插件名。确认插件名时禁止读取 `.pro`。
3. 打包客户端插件时确认架构，并确认是否按多架构目录存放。只打包控制器插件时不确认客户端架构。

上述事项确认完成后，按「整理文件」继续。

## 整理文件

先按已经确认的结构读取示例，再放入文件。未要求多架构时，读取 [references/single-arch.md](references/single-arch.md)。要求多架构时，读取 [references/multi-arch.md](references/multi-arch.md)。

客户端插件和控制器插件分别按下列方式放入文件：

### 客户端插件

放入插件库、同后缀的 `xplugin`、插件名加 `.json`。`xplugin` 从客户端二次开发包的 `lib/windows` 或 `lib/aarch64` 复制。开发包路径未给出，且构建目录、插件工程目录、打包工作目录中都没有 `xplugin` 时，列入缺失项，禁止自行搜索开发包。配置文件不存在时，先询问是否生成。`name` 使用客户端插件名。

### 控制器插件

放入 `.so`、`.json`、`.lic`。`.lic` 使用珞石提供的文件，禁止生成。配置文件不存在时，先询问是否生成。`name` 使用控制器插件名。

查找范围限于用户给出的构建目录、插件工程目录、打包工作目录，以及编译已经给出的库路径。禁止在上述目录以外搜索。找到一份就复制。找到多份时列出绝对路径，请用户选定。确认前禁止复制。一处都没有时列出目标绝对路径，请用户放入。已经备齐的另一边可以先放好。库文件尚未放入时停在本节。

`version` 等请求里没有的字段，先向用户确认再写入。尚未列入本次打包的插件目录先移出 `client/` 或 `controller/`，再运行校验。

## 运行打包脚本

打包工作目录中没有对应脚本时，从本技能 `assets/` 复制。Windows 使用 [assets/PluginPackage.bat](assets/PluginPackage.bat)，Linux 使用 [assets/PluginPackage.sh](assets/PluginPackage.sh)。同名脚本已经存在时禁止覆盖。

先在本技能目录运行 `scripts/check_stage.py`。命令中的路径和插件名换成实际值。有多个插件时，重复对应的 `--client-name` 或 `--controller-name`。只打包控制器插件时，不写 `--client-name`、`--client-suffix`、`--client-arch`。用户要求多架构时，重复对应的 `--client-arch` 或 `--controller-arch`。给出架构参数时，校验 `client/架构/插件名/` 和 `controller/架构/插件名/`，不必再写 `--client-suffix`。

```bash
python scripts/check_stage.py 打包工作目录 --package-os windows --client-suffix dll --client-name ClientPlugin --controller-name ControllerPlugin
python scripts/check_stage.py 打包工作目录 --package-os linux --client-suffix so --client-name ClientPlugin --controller-name ControllerPlugin
python scripts/check_stage.py 打包工作目录 --package-os linux --client-name MyPlugin --client-arch aarch64 --client-arch amd64 --client-arch win --controller-name MyPlugin --controller-arch aarch64 --controller-arch amd64
```

输出 `OK` 后，在打包工作目录运行脚本。Windows 执行 `PluginPackage.bat`，Linux 执行 `bash PluginPackage.sh`。脚本打印 `[OK] Done` 和插件包路径后等待按键，向脚本发送一次回车。未输出 `OK` 时禁止运行打包脚本。

`python` 命令不存在，或版本不是 Python 3 时，禁止运行校验脚本，禁止安装 Python，禁止在打包工作目录以外查找解释器，禁止运行 `PluginPackage.bat` 或 `PluginPackage.sh`。

## 打包成功

给出插件包 zip 的绝对路径。

## 打包失败

打包无法继续时立即停止。先列出缺少或不符项的绝对路径，再写简要原因，并列出可选处理。未获用户进一步指示前，禁止运行打包脚本。
