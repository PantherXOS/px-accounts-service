import pkgutil

if pkgutil.find_loader('PluginFramework') is not None:

    import PluginFramework

    class Plugin(PluginFramework.Plugin):

        title = 'imap'

        def __init__(self):
            pass

        def verify(self, params):
            result = PluginFramework.VerifyResult()
            result.verified = True

            required_params = [ 'user', 'remotehost', 'ssl', 'starttls', 'password' ]
            optional_params = { 'remoteport': '143', 'autorefresh': '30', 'maxsize': '1000' }
            protected_params = [ 'password' ]

            keys = list()
            for k in params:
                keys.append(k)

            for p in required_params:
                if p not in keys:
                    result.errors.append('\'{}\' is required, but not provided in params'.format(p))
                    result.verified = False

            if result.verified:
                for k in required_params:
                    p = PluginFramework.ServiceParam()
                    p.key = k
                    p.val = params[k]
                    p.is_required = True
                    if k in protected_params:
                        p.is_protected = True
                    result.params.append(p)

                for k in optional_params:
                    p = PluginFramework.ServiceParam()
                    p.key = k
                    if k in keys:
                        p.val = params[k]
                    else:
                        p.val = optional_params[k]

                    p.is_required = False
                    if k in protected_params:
                        p.is_protected = True
                    result.params.append(p)

            return result

        def authenticate(self, params):
            result = PluginFramework.AuthResult()
            result.authenticated = False
            result.errors.append('method not implemented.')
            return  result
