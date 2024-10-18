-- Index on Users table to improve login query performance
CREATE INDEX idx_users_login ON Users(login);
-- Index on Catalog table to improve game lookup performance
CREATE INDEX idx_catalog_gameid ON Catalog(gameID);
-- Index on RentalOrder table to improve order lookup performance by user
CREATE INDEX idx_rentalorder_login ON RentalOrder(login);
-- Index on TrackingInfo table to improve tracking lookup performance
CREATE INDEX idx_trackinginfo_trackingid ON TrackingInfo(trackingID);
