create table esp32_data(
    esp_id bigint IDENTITY,
    esp_readingid bigint NULL,
    esp_datacreated date Null,
    esp_data FLOAT NULL
)
-- drop table esp32_data

--delete from esp32_data
select * from esp32_data