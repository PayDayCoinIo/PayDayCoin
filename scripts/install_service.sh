#!/bin/bash
NAME=paydayd
SERVICE_FILE=$PWD/init-script

echo "--- Installation Service PayDayd ---"
if [ ! -w /etc/init.d ]; then
  echo "You don't gave me enough permissions to install service myself."
  echo "That's smart, always be really cautious with third-party shell scripts!"
  echo "You should now type those commands as superuser to install and run your service:"
  echo ""
  echo "   cp \"$SERVICE_FILE\" \"/etc/init.d/$NAME\""
  echo "   chmod a+x \"/etc/init.d/$NAME\""
  echo "   update-rc.d \"$NAME\" defaults"
else
  echo "1. Copy script \"$SERVICE_FILE\" to \"/etc/init.d/$NAME\""
  cp  $SERVICE_FILE /etc/init.d/$NAME
  echo "2. Set: chmod a+x \"/etc/init.d/$NAME\""
  chmod a+x /etc/init.d/$NAME
  echo "3. Run: update-rc.d \"$NAME\" defaults"
  update-rc.d $NAME defaults
  systemctl daemon-reload
fi
echo "--- Servive PayDayd installed successful ---"
echo "--- To remove service: run uninstall_service.sh ---"
echo "--- Exit ---"
