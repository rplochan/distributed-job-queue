
-- Create jobs table
CREATE TABLE IF NOT EXISTS jobs (
    id INTEGER PRIMARY KEY,
    type VARCHAR(50) NOT NULL,

    input_data JSONB NOT NULL DEFAULT '{}'::jsonb,

    status VARCHAR(20) NOT NULL DEFAULT 'pending',

    result_int BIGINT,
    result_text TEXT,
    error TEXT,

    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT valid_job_status
        CHECK (status IN ('pending', 'processing', 'completed', 'failed'))
);

-- Index for filtering jobs by status
CREATE INDEX IF NOT EXISTS idx_jobs_status
    ON jobs(status);

-- Index for sorting/filtering jobs by creation time
CREATE INDEX IF NOT EXISTS idx_jobs_created_at
    ON jobs(created_at);
