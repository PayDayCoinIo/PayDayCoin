#!/bin/bash
NAME=paydayd
SERVICE_FILE=/etc/init.d/$NAME
PIDFILE=/var/run/PayDayd.pid

echo "--- Uninstallation Service PayDayd ---"
if [ ! -w /etc/init.d ]; then
  echo "You don't gave me enough permissions to uninstall service myself."
  echo "That's smart, always be really cautious with third-party shell scripts!"
  echo "You should now type those commands as superuser to uninstall and remove your service:"
  echo " === !!! Service will be terminated !!! === "
  echo "   update-rc.d -f \"$NAME\" remove"
  echo "   rm -fv \"$SERVICE_FILE\""
  echo "   systemctl daemon-reload"
  echo ""
else
  if [ -f $PIDFILE ] && kill -0 $(cat $PIDFILE); then
    kill -15 $(cat "$PIDFILE") && rm -f "$PIDFILE"
  fi

  echo "1. Run: update-rc.d -f \"$NAME\" remove"
  update-rc.d -f $NAME remove
  echo "2. Run: rm -fv \"$SERVICE_FILE\""
  rm -fv $SERVICE_FILE
  echo "3. Run: systemctl daemon-reload"
  systemctl daemon-reload
fi
echo ""
echo "--- Servive PayDayd uninstalled successfull ---"
echo "--- Exit ---"
