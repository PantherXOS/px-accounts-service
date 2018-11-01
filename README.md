---
category: desktop
developer: Reza Alizadeh Majd <r.majd@pantherx.org>
priority: 10
packages:
  - px_accounts
related:
  - px_accounts_ui
  - px_accounts_service
  - px_accounts_helper_imap
  - px_accounts_helper_ftp
  - px_accounts_helper_btc
dependencies:
  - lib_px_accounts
---

# Accounts

  - `px_accounts_ui` **Online Accounts** ui-application (frontend, only UI)
  - `px_accounts` application (add, modify, verify, delete)
  - `px_accounts_service` background-service (track account status)
  - [`lib_px_accounts`](https://git.pantherx.org/development/applications/lib_px_accounts) authenticate applications to retrieve account related passwords
  - `px_accounts_helper_imap` verify IMAP credentials (1)
  - `px_accounts_helper_ftp` verify FTP credentials
  - `px_accounts_plugin_btc` Bitcoin support (2, 3)
  - `px_accounts_plugin_eth` Ethereum support
  - ...

(1) a small helper script, that either reports `OK` or `ERROR(message)`  
(2) plugins support additional features  
(3) cryptocurrency support will initially be limited to account balance and transfer notifications

## Important features

  1. Add new account
  2. Edit accounts
  3. Delete account
  4. provide authentication for pass
  5. Track account status

## Implementation

```mermaid
graph TD
A>px_accounts_service]
B>px_accounts]
C>px_accounts_ui]
E[/accounts]
F>px_pass]
G[modules]
H>px_accounts_helper_*]
I>px_accounts_plugin_*]
J(Server)
K[Crypto, VPN, Proxy, 3rd party]

B --> |file system| E
B -.- |RPC| F
B --> |extent| G
G --> H
G --> I
C -.- |RPC| B
A --> |file system| E
H --> |verify credentials| J
I --> |support new account types| K
```

**Accounts** manages all app-related accounts such as Email, Calendar, Contacts, Matrix, IRC, Dropbox, Telegram, among others.

  - Configuration is stored in `~/.userdata/accounts` (file_name: `<account-name>.scm`)
  - Credentials are stored using `px_pass`, via RPC
  - Applications (ex.: `px_mail_service`) request account credentials via RPC `px_pass`
  - Add, modify or remove accounts trough `px_accounts`
  - `px_accounts_ui` UI for `px_accounts` via RPC

**Q: What does the `px_accounts_ui` do?**

  - UI for `px_accounts`
  - does not run in the background.
  - Execution of commands trough `px_accounts`

**Q: What does the `px_accounts` do?**

  - Read and modify accounts stored in `~/.userdata/accounts`
  - programmatic access, ex.: `px_accounts add -provider fastmail -account "info@pantherx.org" "abcdef"`
  - Stores passwords in `px_pass`
  - provides RPC access for 3rd party applications (access accounts)

When you add a account using `-provider`  
(1) without defining protocols; ex.: `-protocol imap caldav carddav`, it's assumed that you want to activate all supported protocols.   
(2) without defining a name, ex.: `-name "Work"` it defaults to the first argument after `-account`. Ex.: `-account "info@pantherx.org" "abcdef"` would result in an account name _info@pantherx.org_ with filename _fastmail_info_at_pantherx_org_.

When you add a account using only `-protocol` (not `-provider`), you can manually define properties such as server address `-server`, port `-port` including other, protocol-related properties such as the `-path` for the IMAP protocol.

**Q: What does the `px_accounts_service` do?**

Keep track of account status with 3 possible states:

  1. online
  2. offline
  3. error
  4. none; never connected

The current state of an account is determined two-ways:

1) by `px_accounts_service` on start-up

  - open `<account-name>.scm` in `~/.userdata/accounts`
  - for existing account, show account but status 'none'

_Any sort of account configuration change, will prompt `px_accounts_service` to reload._

2) application reporting (ex. `px_mail_service`)

  - once services come online, and syncs successfully, report _online_
  - should sync fail, report _error_
  - should connection fail, report _offline_

This is one-way, possibly via socket.

**Q: How does accounts verify newly added accounts?**

  example:

  1. `px_accounts add -protocol sftp -account "abc" "abcdef" -server "192.168.1.20" -port "22"`
  2. `px_accounts` now calls `px_accounts_helper_ftp` helper script
  1. `px_accounts_plugin_ftp` returns `OK` or `ERROR(message)`

    ping <service> with <authentication>
      if accepted
        return OK
      if not accepted
        return <error>

**Q: Why do we use so many helper scripts?**

We could easily ask `px_mail_service` to verify the credentials, however:

  - `px_accounts` should be able to verify credentials independently from `px_mail_service`
  - `px_accounts_plugin_imap` borrows code from `px_mail_service`, and re-uses it's libraries

### Supported Services

#### Core

**by protocol / type**

  - POP (1)
  - IMAP
  - SMTP
  - CalDAV
  - CardDAV
  - SFTP
  - OpenVPN
  - Cryptocurrency (2)
    - BTC
    - ETH
    - XRP

(1) Is POP support still viable / necessary?  
(2) Initially we'll only store the currency address, and retrieve balance and transactions history.

**by provider**

(EMA)il, (CAL)endar, (CON)tacts

  - Google (EMA,CAL,CON)
  - Yahoo (EMA)
  - FastMail (EMA,CAL,CON)
  - iCloud (EMA,CAL,CON)
  - Gandi (EMA)
  - Office 365 (EMA,CAL,CON)
  - Outlook.com (EMA,CAL,CON)

(OPE)nVPN

  - TorGuard (OPE)
  - AzireVPN (OPE)
  - AirVPN (OPE)