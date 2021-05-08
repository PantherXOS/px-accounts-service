# Changelog

All notable changes to this project will be documented in this file.

## [0.4.0]
### Changes
- add support for optional provider services
- improve logs, mask secret strings
- use $XDG_DATA_HOME as prefix for default accounts path
- update RPC path for px-accounts-service
- update RPC path for px-events-service
- update RPC path for px-secret-service
 
## [0.3.6]
### Fixed
- update internal plugin definition to work with `v0.3.5` changes

## [0.3.5]
### Fixed
- Issue related to loading python plugins. (define __init__ method to PluginFramework.Plugin class).

## [0.3.4]
### Changed
- update secret labels to a meaningful value
- updated error messages to be more uniform

## [0.3.3]
### Changed
- add `services` param to Account events.

### Fixed
- Fix account creation event trigger issue.

## [0.3.2]
### Fixed
- issue related to remove secrets during account removal.
- fix add secret issue for dual_password schema.

## [0.3.1]
### Fixed
- issue related to initiating the account id

## [0.3.0]
### Changed
- integration with new implementation of px-secret-service

## [0.2.13]

### Added
- Multiple paths for account files + support for readonly account paths

### Fixed
- build issue related to new version of GCC fixed.
- High cpu usage issue related to thread timeouts fixed.

## [0.2.12]

### Fixed
- plugin load issue for relative paths fixed

## [0.2.11]

### Added
- load custom plugin definitions using `PLUGIN_PATH`

## [0.2.10]

### Fixed
- Update Secret service RPC interface #66
- Fix Application crash issue during plugin initiation #65
