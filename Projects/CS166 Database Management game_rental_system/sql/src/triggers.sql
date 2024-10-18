-- Triggers and Stored Procedures

-- Trigger Function to increment the number of overdue games
CREATE OR REPLACE FUNCTION increment_overdue_games()
RETURNS TRIGGER AS $$
BEGIN
    UPDATE Users
    SET numOverDueGames = numOverDueGames + 1
    WHERE login = NEW.login;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Trigger to call the increment_overdue_games function after inserting into rentalorder
CREATE TRIGGER rental_order_placed
AFTER INSERT ON rentalorder
FOR EACH ROW
EXECUTE PROCEDURE increment_overdue_games();

-- Stored Procedure to place an order
CREATE OR REPLACE FUNCTION place_order_procedure(user_login VARCHAR, game_ids VARCHAR)
RETURNS VOID AS $$
DECLARE
    rentalOrderID UUID := uuid_generate_v4();
    trackingID UUID := uuid_generate_v4();
    noOfGames INT := array_length(string_to_array(game_ids, ','), 1);
    totalPrice DECIMAL := 0.0;
    gameID VARCHAR;
BEGIN
    -- Calculate total price of the games
    FOR gameID IN SELECT * FROM unnest(string_to_array(game_ids, ',')) LOOP
        SELECT price INTO totalPrice FROM Catalog WHERE gameID = gameID;
        totalPrice := totalPrice + totalPrice;
    END LOOP;

    -- Insert new rental order
    INSERT INTO RentalOrder (rentalOrderID, login, noOfGames, totalPrice, orderTimestamp, dueDate)
    VALUES (rentalOrderID, user_login, noOfGames, totalPrice, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP + INTERVAL '7 days');

    -- Insert tracking information
    INSERT INTO TrackingInfo (trackingID, rentalOrderID, status, currentLocation, courierName, lastUpdateDate, additionalComments)
    VALUES (trackingID, rentalOrderID, 'Processing', 'Warehouse', 'Courier Service', CURRENT_TIMESTAMP, '');

    -- Insert games into gamesinorder
    FOR gameID IN SELECT * FROM unnest(string_to_array(game_ids, ',')) LOOP
        INSERT INTO GamesInOrder (rentalOrderID, gameID, unitsOrdered)
        VALUES (rentalOrderID, gameID, 1);
    END LOOP;
END;
$$ LANGUAGE plpgsql;
