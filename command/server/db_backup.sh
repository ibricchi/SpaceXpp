mkdir -p /home/ubuntu/SpaceXpp/command/server/db_backup
tar -czf /home/ubuntu/SpaceXpp/command/server/db_backup/db_daily_$(date -u +%Y%m%d).tar.gz /home/ubuntu/SpaceXpp/command/server/db
